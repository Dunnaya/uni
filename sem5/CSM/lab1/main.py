import numpy as np
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt

def discreteFourierTransform(samples: np.ndarray) -> np.ndarray:
    N = len(samples)
    m = np.arange(N)
    k = m.reshape((N, 1))
    e = np.exp(-2j * np.pi * k * m / N)
    return np.dot(e, samples)

def testDFT(samples: np.ndarray) -> None:
    customDFTres = discreteFourierTransform(samples)
    npFFTres = np.fft.fft(samples)

    maxAbsoluteError = np.max(np.abs(customDFTres - npFFTres))

    if(maxAbsoluteError < 1e-6):
        print("Custom DFT is ok")
    else:
        print("You're stupid af!")
        print(f"Max absolute error: {maxAbsoluteError}")

def findFreqContribution(samples: np.ndarray, observInterval: int = 5):
    numSamples = len(samples)
    """
    I'm tired.
    """