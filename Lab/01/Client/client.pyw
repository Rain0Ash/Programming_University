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
    title_bar_height = win.winfo_rooty() - win.winfo_y()
    win_height = height + title_bar_height + frm_width
    x = win.winfo_screenwidth() // 2 - win_width // 2
    y = win.winfo_screenheight() // 2 - win_height // 2
    win.geometry('{}x{}+{}+{}'.format(width, height, x, y))
    win.deiconify()


def _asyncio_thread(loop, function):
    try:
        loop.run_until_complete(function)
    except RuntimeError:
        pass


def do_async(function):
    threading.Thread(target=_asyncio_thread, args=(async_loop, function)).start()


def get_request() -> str:
    try:
        return requests.get('http://localhost:3000/raw').content.decode("utf8")
    except requests.exceptions.ConnectionError:
        return None


async def get_request_async() -> str:
    return get_request()


def get_json():
    try:
        data = get_request()
        return json.loads(data) if data is not None else None
    except Exception:
        return None


async def get_json_async():
    return get_json()


def update_gui(data):
    if data is None:
        desc_label.config(text="Нет данных.")
        temp_label.config(text="")
        return

    desc_label.config(text=str(data["description"]))
    temp_label.config(text=str(data["temp"]) + "°C")


async def on_form_click_async(event):
    data = await get_json_async()
    update_gui(data)


def on_form_click_callback(event):
    try:
        do_async(on_form_click_async(event))
    except RuntimeError:
        pass


tick_cooldown = 30000


def tick_callback():
    root.after(tick_cooldown, tick_callback)
    return on_form_click_callback(None)


yellow = "#ffbb50"
white = "#ffffff"

if __name__ == '__main__':
    async_loop = asyncio.get_event_loop()
    root = Tk()
    root.title("Погода")
    root.resizable(0, 0)
    root.pack_propagate(0)

    root.bind("<Button-1>", on_form_click_callback)

    w = 250
    h = 170
    root.geometry(f"{w}x{h}")

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

    root.after(0, tick_callback)

    center(root)
    root.mainloop()
