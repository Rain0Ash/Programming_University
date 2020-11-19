#!/usr/bin/python
# -*- coding: utf-8 -*-

from tkinter import Tk, Frame, BOTH, TOP, BOTTOM, X, Y, Label
import json
import requests
import asyncio
import threading

import warnings
warnings.filterwarnings("ignore")

def center(win):
    win.update_idletasks()
    width = win.winfo_width()
    frm_width = win.winfo_rootx() - win.winfo_x()
    win_width = width + 2 * frm_width
    height = win.winfo_height()
    titlebar_height = win.winfo_rooty() - win.winfo_y()
    win_height = height + titlebar_height + frm_width
    x = win.winfo_screenwidth() // 2 - win_width // 2
    y = win.winfo_screenheight() // 2 - win_height // 2
    win.geometry('{}x{}+{}+{}'.format(width, height, x, y))
    win.deiconify()


def _asyncio_thread(async_loop, function):
    try:
        async_loop.run_until_complete(function)
    except RuntimeError:
        pass


def do_async(function):
    threading.Thread(target=_asyncio_thread, args=(async_loop, function)).start()


def get_request() -> str:
    return requests.get('http://localhost:3000/raw').content.decode("utf8")


async def get_request_async() -> str:
    return get_request()


def get_json():
    return json.loads(get_request())


async def get_json_async():
    return get_json()


def update_gui(data):
    desc_label.config(text=str(data["description"]))
    temp_label.config(text=str(data["temp"]) + "°C")


def on_form_click(event):
    data = get_json()
    update_gui(data)


async def on_form_click_async(event):
    data = await get_json_async()
    update_gui(data)


def on_form_click_callback(event):
    try:
        do_async(on_form_click_async(event))
    except RuntimeError:
        pass


yellow = "#ffbb50"
white = "#ffffff"

if __name__ == '__main__':
    async_loop = asyncio.get_event_loop()
    root = Tk()
    root.title("Погода")
    root.resizable(0, 0)
    root.pack_propagate(0)

    root.bind("<Button-1>", on_form_click_callback)

    w = 200
    h = 150

    top_frame = Frame(root, bg=yellow, width=w, height=h * 0.2)
    middle_frame = Frame(root, bg=white, width=w, height=h * 0.6)
    bottom_frame = Frame(root, bg=yellow, width=w, height=h * 0.2)

    top_frame.pack(side=TOP, fill=X)
    middle_frame.pack(expand=True, fill=BOTH)
    bottom_frame.pack(side=BOTTOM, fill=X)

    city_label = Label(top_frame, font=("Calibri Bold", 12), text="Симферополь", bg=yellow)
    desc_label = Label(top_frame, font=("Calibri", 12), bg=yellow)
    temp_label = Label(middle_frame, font=("Impact", 48), bg="#ffffff")

    city_label.pack(pady=0)
    desc_label.pack(pady=0)
    temp_label.pack(expand=True)

    on_form_click(None)
    center(root)
    root.mainloop()
