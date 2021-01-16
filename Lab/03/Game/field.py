class ModelField(list):
    def __init__(self, field: list):
        super().__init__()
        self.field = field

        size = len(field)

        def get_square_diag(square_shift_x, square_shift_y, square_size, reversed=False):
            nonlocal field

            diagonal = list()
            for i in range(square_size):
                zx = i if not reversed else (square_size - 1 - i)
                zy = i

                diagonal.append(field[square_shift_y + zy][square_shift_x + zx])

            return diagonal

        rows = field

        columns = [list() for _ in field[0]]
        for row in field:
            i = 0
            for cell in row:
                columns[i].append(cell)
                i += 1

        diagonal = list()
        diagonal.append(get_square_diag(0, 0, size))
        for i in range(5, size):
            k = size - i
            diagonal.append(get_square_diag(k, 0, i))
            diagonal.append(get_square_diag(0, k, i))

        subdiagonal = list()
        subdiagonal.append(get_square_diag(0, 0, size, True))
        for i in range(5, size):
            k = size - i
            subdiagonal.append(get_square_diag(k, k, i, True))
            subdiagonal.append(get_square_diag(0, 0, i, True))

        self.rows = rows
        self.columns = columns
        self.diagonal = diagonal
        self.subdiagonal = subdiagonal


if __name__ == "__main__":
    from start import start

    start()