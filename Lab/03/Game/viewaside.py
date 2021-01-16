from tkinter import *


class ViewAside:
    class ViewAsideInformation:
        def __init__(self, master, background: str = None, foreground: str = None):
            self.background = background if background else '#303030'
            self.foreground = foreground if foreground else '#ffffff'
            self.score_label = Label(master, bg=self.background, fg=self.foreground)
            self.score_label.grid(row=1, column=1, columnspan=3)
            self.lines_label = Label(master, bg=self.background, fg=self.foreground)
            self.lines_label.grid(row=2, column=1, columnspan=3)
            self.update_lines_label(0)
            self.update_score(0)

        def update_lines_label(self, value):
            self.lines_label.config(text=f'Линии: {value}')

        def update_score(self, value):
            self.score_label.config(text=f'Счёт: {value}')

    class ViewAsidePrompt(list):
        def __init__(self, master, background: str = None, foreground: str = None):
            super().__init__()
            self.background = background if background else '#303030'
            self.foreground = foreground if foreground else '#eeeeee'
            Label(master, height=2, background=self.background).grid(row=4, column=1, columnspan=3)
            Label(master, text='Будут добавлены:', bg=self.background, fg=self.foreground).grid(row=5, column=1, columnspan=3)
            self.model = list()
            for i in range(3):
                self.append(Label(master, bg=self.background))
                self.model.append(None)
                self[i].grid(row=6, column=1 + i)

        def update(self: list, balls: list):
            for i, ball in enumerate(balls):
                self[i].config(image=ball.take(5))
                self.model[i] = ball

    class ViewAsideButtons:
        def __init__(self, master, background: str = None, foreground: str = None):
            self.background = background if background else '#6e7078'
            self.foreground = foreground if foreground else '#eeeeee'
            self.next_step_button = Button(master, text="Сделать пустой ход", bg=self.background, fg=self.foreground, padx=20, pady=10,
                                           relief='flat',
                                           width=15)
            self.next_step_button.grid(row=7, column=1, columnspan=3)

            Label(master, height=2, bg='#303030').grid(row=8, column=1, columnspan=3, pady=50)

            self.new_game_button = Button(master, text="Новая игра", bg=self.background, fg=self.foreground, padx=20, pady=10,
                                          relief='flat',
                                          width=15)
            self.new_game_button.grid(row=10, column=1, columnspan=3)

    def __init__(self, master):
        master.config(bg='#303030')

        self.information = ViewAside.ViewAsideInformation(master)
        self.prompt = ViewAside.ViewAsidePrompt(master)
        self.buttons = ViewAside.ViewAsideButtons(master)


if __name__ == "__main__":
    from start import start
    start()
