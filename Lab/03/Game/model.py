from random import randint
from point import Point
from view import View, ViewCell
from field import ModelField
from enum import Enum


class State(Enum):
    BallNotSelected = 0
    BallSelected = 1
    BallMoved = 2
    BeforeGameOver = 3
    GameOver = 4


def get_random(list_of):
    return list_of[randint(0, len(list_of) - 1)]


class Model:
    def __init__(self, view: View):
        self.view = view
        self.state = State.BallNotSelected
        self.selected = None
        self.score = 0
        self.lines = 0
        self.field = ModelField(view.fields)

        self.restart()

        view.view_aside.buttons.new_game_button.bind('<Button-1>', self.restart)
        view.view_aside.buttons.next_step_button.bind('<Button-1>', self.step)

        for row in self.view.fields:
            for cell in row:
                cell.on_click_handler = self.__cell_click_handler__

    def restart(self, event=None):
        self.view.game_over_visibility = False
        self.clear_field()
        self.generate()
        self.step()
        self.view.view_aside.information.update_score(0)
        self.view.view_aside.information.update_lines_label(0)
        self.state = State.BallNotSelected

    def step(self, event=None):
        if self.state == State.GameOver:
            return

        self.use()
        self.generate()

    def generate(self):
        balls = list()
        for i in range(3):
            balls.append(self.random_ball())
        self.view.view_aside.prompt.update(balls)

    def use(self):
        hint = self.view.view_aside.prompt
        free = self.get_free_cells()

        for i in range(2, -1, -1):
            if len(free) - i <= 1:
                self.state = State.BeforeGameOver
                self.game_over()
                return
            else:
                cell = get_random(free)
                cell.put_ball(hint.model[i])
                free.pop()

    def game_over(self):
        self.view.game_over_visibility = True
        self.state = State.GameOver

    def random_ball(self):
        tiles = self.view.tiles

        balls = (
            tiles.ball_aqua,
            tiles.ball_blue,
            tiles.ball_green,
            tiles.ball_pink,
            tiles.ball_red,
            tiles.ball_violet,
            tiles.ball_yellow
        )

        return get_random(balls)

    def get_free_cells(self):
        free = list()
        field = self.view.fields
        for row in field:
            for cell in row:
                if cell.is_free:
                    free.append(cell)
        return free

    def clear_field(self):
        field = self.view.fields

        for row in field:
            for cell in row:
                cell.clear()

    def __cell_click_handler__(self, cell: ViewCell, event):
        if cell.is_free:
            if self.state == State.BallNotSelected:
                pass
            elif self.state == State.BallSelected:
                reachable = self.cells_reachable(self.selected, cell)

                if reachable:
                    cell.put_ball(self.selected.tile)

                    self.selected.unselect_ball()
                    self.selected.clear()
                    self.selected = None

                    self.state = State.BallMoved

                    lines = self.check_all_lines()

                    for line in lines:
                        self.lines += 1
                        self.view.view_aside.information.update_lines_label(self.lines)

                        for cell in line:
                            cell.unselect_ball()
                            cell.clear()

                            self.score += 2
                            self.view.view_aside.information.update_score(self.score)

                    self.state = State.BallNotSelected

                    if len(lines) == 0:
                        self.step()

                else:
                    pass
        else:
            if self.state == State.BallNotSelected:
                self.state = State.BallSelected
                self.selected = cell
                cell.select_ball()
            elif self.state == State.BallSelected:
                self.selected.unselect_ball()
                self.selected = cell
                cell.select_ball()

    def cells_reachable(self, first: ViewCell, second: ViewCell, way=False):
        def reset_cells_previous():
            field = self.view.fields
            for row in field:
                for cell in row:
                    cell.is_prev = False

        def step_all_directions(from_cell: ViewCell, target: ViewCell):
            steps = list()

            left = Point(1, 0)
            right = Point(-1, 0)
            up = Point(0, 1)
            down = Point(0, -1)
            for direction in (up, down, left, right):
                intersection = from_cell.intersection(direction)

                if not intersection:
                    continue

                if intersection == target:
                    return True

                if intersection.is_free and not intersection.previous:
                    intersection.previous = True
                    if way:
                        intersection.ball_background_blend(1, self.view.tiles.ball_yellow, 6)
                    steps.append(intersection)

                elif intersection.is_free and intersection.previous:
                    if way:
                        intersection.ball_background_blend(1, self.view.tiles.ball_green, 6)

            return steps

        steps = [first]

        while True:
            new_steps_storage = list()

            for step in steps:
                new_steps = step_all_directions(step, second)

                if new_steps is True:
                    reset_cells_previous()
                    return True

                new_steps_storage += new_steps

            if len(new_steps_storage) == 0:
                reset_cells_previous()
                return False

            steps = new_steps_storage

    def check_all_lines(self):

        def check_lines(lines):
            sequences = list()

            for line in lines:
                sequence = list()
                previous = None

                def reset_sequence(color, cell=None):
                    nonlocal sequence, previous

                    if len(sequence) > 4:
                        sequences.append(sequence)

                    previous = color
                    sequence = [cell] if cell else list()

                for cell in line:
                    if cell.is_free:
                        reset_sequence(None)
                        continue

                    current = cell.tile

                    if previous is None:
                        reset_sequence(current, cell)
                    elif current == previous:
                        sequence.append(cell)
                    else:
                        reset_sequence(current, cell)

                reset_sequence(None)

            return sequences

        lines = list()
        lines += check_lines(self.field.rows)
        lines += check_lines(self.field.columns)
        lines += check_lines(self.field.diagonal)
        lines += check_lines(self.field.subdiagonal)

        return lines


if __name__ == "__main__":
    from start import start
    start()
