from tkinter import *
from PIL import Image, ImageTk
from os import path
from point import Point
from viewaside import ViewAside

imgpath = path.realpath(path.dirname(__file__) + '/img')


class Tile:
    def __init__(self, name, size):
        self._image_path = imgpath + f'/{name}.png'
        self.normal_mode()
        self.size = size
        self.taken = [None] * size

    def rgba_mode(self):
        self.image = Image.open(self._image_path).convert('RGBA')
        return self

    def normal_mode(self):
        self.image = Image.open(self._image_path)
        return self

    def take(self, i: int):
        taken = self.taken[i]

        if taken:
            return taken

        taken = self.cut(i)
        image = ImageTk.PhotoImage(taken)
        self.taken[i] = image
        return image

    def cut(self, i):
        return self.image.crop((1, (i - 1) * self.size, self.size, i * self.size))


class ViewTiles:
    def __init__(self):
        self.ball_aqua = Tile('ball-aqua', 60)
        self.ball_blue = Tile('ball-blue', 60)
        self.ball_green = Tile('ball-green', 60)
        self.ball_pink = Tile('ball-pink', 60)
        self.ball_red = Tile('ball-red', 60)
        self.ball_violet = Tile('ball-violet', 60)
        self.ball_yellow = Tile('ball-yellow', 60)

        self.cell = Tile('cell-bgr', 69)
        self.page = Tile('page-bgr', 128)


tiles = None


class ViewCell(Label):
    def __init__(self, master, row, col, field):
        super().__init__(master, image=tiles.cell.take(1), borderwidth=0)
        self.image = None
        self.tile = None
        self.field = field
        self.grid(row=row, column=col)
        self.bind('<Button-1>', self.on_click)
        self.on_click_handler = None
        self.position = Point(col, row)
        self.previous = False

    def put_ball(self, tile: Tile):
        self.ball_background_blend(1, tile, 1)

    def select_ball(self):
        self.ball_background_blend(2, self.tile, 1)

    def unselect_ball(self):
        self.ball_background_blend(1, self.tile, 1)

    def ball_background_blend(self, bg_tile_num, tile, tile_num):
        bg = tiles.cell.rgba_mode().cut(bg_tile_num)
        ball = tile.rgba_mode().cut(tile_num)
        nball = Image.new('RGBA', bg.size)
        nball.paste(ball, (7, 6))
        combined = ImageTk.PhotoImage(Image.alpha_composite(bg, nball))
        self.image = combined
        self.tile = tile
        self.config(image=combined)

    def clear(self):
        self.image = None
        if tiles is not None and tiles.cell is not None:
            self.config(image=tiles.cell.take(1))

    def on_click(self, event):
        if self.on_click_handler:
            self.on_click_handler(self, event)

    @property
    def is_free(self):
        return not self.image

    def intersection(self, direction: Point):
        npos = self.position.different(Point(-direction.x, -direction.y))

        if (npos.x < 0 or npos.y < 0 or
                npos.y > len(self.field) - 1 or npos.x > len(self.field[0]) - 1):
            return None

        return self.field[npos.y][npos.x]


class View:
    def __init__(self, count):
        global tiles

        self.root = Tk()
        aside_width = 200
        width = 68 * count + aside_width
        height = 68 * count + 10
        self.root.title("Три в ряд")
        self.size = Point(width, height)
        self.root.geometry(f'{width}x{height}')
        self.count = count

        tiles = ViewTiles()
        self.tiles = tiles

        self.root.config(background='#303030')

        self.field = Frame(self.root)
        self.field.grid(row=1, column=1)

        self.aside = Frame(self.root, width=aside_width)
        self.aside.grid(row=1, column=2)
        self.view_aside = ViewAside(self.aside)

        self.fields = list()
        for row in range(count):
            self.fields.append(list())
            for column in range(count):
                cell = ViewCell(self.field, row, column, self.fields)
                self.fields[row].append(cell)

        self.center()

        self.game_over = Label(self.field, text="Игра окочена", fg='white', bg='#414141',
                               font=('sans-serif', 30, 'bold'))

    @property
    def game_over_visibility(self):
        pass

    @game_over_visibility.setter
    def game_over_visibility(self, visible: bool):
        if visible:
            self.game_over.place(x=self.size.x / 4.8, y=self.size.y / 2.2)
        else:
            self.game_over.place_forget()

    def start(self):
        self.root.mainloop()

    def center(self):
        self.root.update_idletasks()
        width = self.root.winfo_width()
        frm_width = self.root.winfo_rootx() - self.root.winfo_x()
        win_width = width + 2 * frm_width
        height = self.root.winfo_height()
        title_bar_height = self.root.winfo_rooty() - self.root.winfo_y()
        win_height = height + title_bar_height + frm_width
        x = self.root.winfo_screenwidth() // 2 - win_width // 2
        y = self.root.winfo_screenheight() // 2 - win_height // 2
        self.root.geometry('{}x{}+{}+{}'.format(width, height, x, y))
        self.root.deiconify()


if __name__ == "__main__":
    from start import start
    start()
