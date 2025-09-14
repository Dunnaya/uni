import numpy as np
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt

def manualDFT(samples: np.ndarray) -> np.ndarray:
    N = len(samples)
    m = np.arange(N)
    k = m.reshape((N, 1))
    e = np.exp(-2j * np.pi * k * m / N)
    return np.dot(e, samples)

def testDFT(samples: np.ndarray) -> None:
    customDFTres = manualDFT(samples)
    npFFTres = np.fft.fft(samples)

    maxAbsoluteError = np.max(np.abs(customDFTres - npFFTres))

    if(maxAbsoluteError < 1e-6):
        print("Custom DFT is ok")
    else:
        print("You're stupid af!")
        print(f"Max absolute error: {maxAbsoluteError}")

def findFreqContribution(samples: np.ndarray, observInterval: int = 5):
    numSamples = len(samples)
    deltaF = 1 / observInterval
    modulesDFT = np.abs(manualDFT(samples)) # or np.fft.fft(samples)
    plotDFTModules(
        modulesDFT,
        deltaF,
        'dft-modules.png'
    )

    localMaxIndices = []
    for k in range(1, numSamples-1):
        if (modulesDFT[k] > modulesDFT[k-1]) and (modulesDFT[k] > modulesDFT[k+1]):
            localMaxIndices.append(k)
    
    frequencies = []
    for k in localMaxIndices:
        freq = k * deltaF
        frequencies.append(freq)
    
    return frequencies

def plotDFTModules(
        modulesDFT: np.ndarray,
        deltaF: float,
        saveImgPath: str
) -> None:
    numSamples = len(modulesDFT)
    # frequencies = []
    # for k in range(1, numSamples):
    #     frequencies.append(k * deltaF)
    
    # plt.plot(frequencies, modulesDFT[1:numSamples])

    halfModules = modulesDFT[:numSamples//2]
    frequencies = np.arange(len(halfModules)) * deltaF
    
    plt.plot(frequencies, halfModules)

    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.savefig(saveImgPath)
    plt.show()

observations = np.loadtxt("f18.txt")
testDFT(observations)
print(findFreqContribution(observations))