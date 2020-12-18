#!/usr/bin/python
# -*- coding: utf-8 -*-

from flask import Flask, request, jsonify
from datetime import datetime as dt
from openpyxl import open as excel_open, Workbook
from openpyxl.styles import Alignment

import os

abspath = os.path.abspath(__file__)
dirname = os.path.dirname(abspath)
os.chdir(dirname)

del os

# Лучше было бы разделить данный класс на два отдельных и использовать паттерн Dependency Injection,
# но из-за размера программы сойдет и такой God Object.
class ExcelBuffer:
    def __init__(self, buffer: list = None, size: int = None, filename: str = None):
        self.buffer = buffer if type(buffer) is list else []
        self.size = int(size) if size and size >= 1 else 1000
        self.filename = filename if filename else "data.xlsx"
        self.book, self.sheet = self.__read__()

    def __iter__(self):
        return self.buffer.__iter__()

    def __getitem__(self, i):
        return self.buffer.__getitem__(i)

    def __len__(self):
        return self.buffer.__len__()

    def __cell__(self) -> int:
        i = 1
        while self.sheet[i][3].value is not None:
            i += 1

        return i
        
    def __number__(self) -> int:
        i = 1
        while self.sheet[i][0].value is not None:
            i += 1

        return i if i < 2 else (i - 1)

    def __table__(self) -> Workbook:
        book = Workbook()
        sheet = book.active

        sheet.column_dimensions['A'].width = 10
        sheet.column_dimensions['B'].width = 80
        sheet.column_dimensions['C'].width = 30
        sheet.column_dimensions['D'].width = 30
        sheet.column_dimensions['E'].width = 20

        sheet['A1'] = "N"
        sheet['B1'] = "User ID"
        sheet['C1'] = "Datetime"
        sheet['D1'] = "Item"
        sheet['E1'] = "Price"

        for i in range(5):
            sheet[1][i].alignment = Alignment(horizontal='center', vertical='center')

        return book

    def __read__(self) -> (Workbook, Workbook):
        try:
            book = excel_open(self.filename, read_only=False)
        except FileNotFoundError:
            book = self.__table__()

        sheet = book.active
        if sheet[1][0].value is None:
            sheet = self.__table__()

        return book, sheet

    def __save__(self):
        empty = self.__cell__()
        number = self.__number__()

        if not len(self):
            return

        for i in range(len(self)):
            self.sheet[empty][0].value = number
            self.sheet[empty][1].value = self.buffer[i]["user_id"]
            self.sheet[empty][2].value = self.buffer[i]["datetime"]

            if len(self.buffer[i]["cart"]):
                for item in self.buffer[i]["cart"]:
                    self.sheet[empty][3].value = item["item"]
                    self.sheet[empty][4].value = item["price"]
                    empty += 1
            else:
                empty += 1
            
            number += 1

        self.book.save(self.filename)
        self.__clear__()

    def __append__(self, item):
        self.buffer.append(item)

    def __clear__(self):
        self.buffer.clear()

    def write(self, item):
        self.__append__(item)
        self.buffer[-1]["datetime"] = dt.now().strftime("%d/%m/%Y %H:%M:%S")

        if len(buffer) >= buffer.size:
            self.__save__()
            self.__clear__()


buffer = ExcelBuffer()


app = Flask(__name__)


@app.route('/', methods=["POST"])
def root():
    if request.is_json:
        buffer.write(request.get_json())
    return "OK"


if __name__ == "__main__":
    app.run()
