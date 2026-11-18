""" Запуск:
    python main.py                 # інтерактивне вікно зі слайдером (тестове зображення)
    python main.py photo.jpg       # з вашим зображенням
    python main.py photo.jpg --table   # таблиця експериментів + збереження графіка
"""
import argparse
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from PIL import Image

from dct import dct_forward, dct_inverse
from dwt import dwt_forward, dwt_inverse, ll_mask
from compression import compress
from metrics import psnr, nonzero_count, compression_ratio

SIZE = 512
LEVELS = 3


def load_image(path):
    if path is None:                      # синтетичне зображення, якщо файл не вказано
        y, x = np.mgrid[0:SIZE, 0:SIZE]
        img = 100 + 80 * x / SIZE + 30 * np.sin(y / 15.0)
        img[(x - 256) ** 2 + (y - 256) ** 2 < 100 ** 2] = 220
        img[100:180, 300:460] = 40
        return np.clip(img, 0, 255)
    img = Image.open(path).convert("L").resize((SIZE, SIZE))
    return np.asarray(img, dtype=float)


def run_dct(img, keep):
    c = compress(dct_forward(img), keep)
    return np.clip(dct_inverse(c), 0, 255), c


def run_dwt(img, keep):
    c = compress(dwt_forward(img, LEVELS), keep, protect=ll_mask(img.shape, LEVELS))
    return np.clip(dwt_inverse(c, LEVELS), 0, 255), c


def print_table(img, keeps=(0.5, 0.2, 0.05, 0.01)):
    print(f"{'Метод':<6}{'keep':>7}{'Ненульових':>13}{'Коеф.стиснення':>16}{'PSNR, дБ':>10}")
    for name, fn in (("DCT", run_dct), ("DWT", run_dwt)):
        for k in keeps:
            rec, c = fn(img, k)
            print(f"{name:<6}{k:>7.0%}{nonzero_count(c):>13}"
                  f"{compression_ratio(c):>16.1f}{psnr(img, rec):>10.2f}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("image", nargs="?")
    ap.add_argument("--table", action="store_true")
    args = ap.parse_args()
    img = load_image(args.image)

    if args.table:
        print_table(img)

    fig, axes = plt.subplots(1, 3, figsize=(15, 6))
    plt.subplots_adjust(bottom=0.2)
    ims = []
    for ax in axes:
        ims.append(ax.imshow(img, cmap="gray", vmin=0, vmax=255))
        ax.axis("off")
    axes[0].set_title("Original")

    def update(keep_percent):
        keep = keep_percent / 100
        for i, (name, fn) in enumerate((("DCT", run_dct), ("DWT", run_dwt)), start=1):
            rec, c = fn(img, keep)
            ims[i].set_data(rec)
            axes[i].set_title(f"{name}\nPSNR = {psnr(img, rec):.2f} дБ | "
                              f"ненульових: {nonzero_count(c)} ({compression_ratio(c):.1f}x)")
        fig.canvas.draw_idle()

    slider = Slider(plt.axes([0.25, 0.07, 0.5, 0.03]), "Зберегти, %", 0.5, 100,
                    valinit=10, valstep=0.5)
    slider.on_changed(update)
    update(10)

    if args.table:
        fig.savefig("result.png", dpi=120)
        print("Збережено result.png")
    plt.show()


if __name__ == "__main__":
    main()