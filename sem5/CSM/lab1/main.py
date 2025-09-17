import numpy as np
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def manualDFT(samples: np.ndarray) -> np.ndarray:
    N = len(samples)
    m = np.arange(N)
    k = m.reshape((N, 1))
    e = np.exp(-2j * np.pi * k * m / N)
    return np.dot(e, samples) / N

def testDFT(samples: np.ndarray) -> None:
    customDFTres = manualDFT(samples)
    npFFTres = np.fft.fft(samples) / len(samples)

    maxAbsoluteError = np.max(np.abs(customDFTres - npFFTres))

    if(maxAbsoluteError < 1e-6):
        print("Custom DFT is ok")
    else:
        print("Oops! Custom DFT is incorrect")
        print(f"Max absolute error: {maxAbsoluteError}")

def findFreqContribution(samples: np.ndarray, observInterval: int = 5):
    numSamples = len(samples)
    deltaF = 1 / observInterval
    modulesDFT = np.abs(manualDFT(samples)) # or np.fft.fft(samples)

    half = numSamples // 2
    localMaxIndices = []
    for k in range(1, half - 1):
        if modulesDFT[k] > modulesDFT[k-1] and modulesDFT[k] > modulesDFT[k+1]:
            localMaxIndices.append(k)

    plotDFTModules(
        modulesDFT,
        deltaF,
        'dft-modules.png',
        peakIndices = localMaxIndices
    )
    
    frequencies = []
    for k in localMaxIndices:
        freq = k * deltaF
        frequencies.append(freq)
    
    return frequencies

def plotDFTModules(
        modulesDFT: np.ndarray,
        deltaF: float,
        saveImgPath: str,
        peakIndices = None
) -> None:
    numSamples = len(modulesDFT)
    # frequencies = []
    # for k in range(1, numSamples):
    #     frequencies.append(k * deltaF)
    # plt.plot(frequencies, modulesDFT[1:numSamples])

    halfModules = modulesDFT[:numSamples//2]
    frequencies = np.arange(len(halfModules)) * deltaF
    
    plt.plot(frequencies, halfModules)

    if peakIndices is not None:
        peakFrequencies = np.array(peakIndices) * deltaF
        peakAmplitudes = halfModules[peakIndices]
        plt.scatter(peakFrequencies, peakAmplitudes, color='red', zorder=5)

    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Amplitude")
    plt.grid(True)
    plt.title("DFT Amplitude Spectrum")
    plt.savefig(saveImgPath)
    plt.show()

def fit_model(t, observations, peak_frequencies):
    def model(t, a1, a2, a3, *params):
        k = len(params) // 2
        y = a1 * t**3 + a2 * t**2 + a3 * t
        for i in range(k):
            fi = params[i]
            ai = params[k + i]
            y += ai * np.sin(2 * np.pi * fi * t)
        return y

    # starting guess for parameters
    initial_guess = [0, 0, 0] + peak_frequencies + [1]*len(peak_frequencies)

    params, covariance = curve_fit(model, t, observations, p0=initial_guess)
    fitted_values = model(t, *params)
    return params, fitted_values

observations = np.loadtxt("f18.txt")
testDFT(observations)

peak_frequencies = findFreqContribution(observations)
print(peak_frequencies)

# time vector
t = np.arange(len(observations)) * 0.01  # delta_t = 0.01

# model fitting
params, fitted_values = fit_model(t, observations, peak_frequencies)
print("Found parameters:", params)

plt.figure(figsize=(12, 6))
plt.plot(t, observations, label='Observations')
plt.plot(t, fitted_values, label='Fitted Model', linestyle='--')
plt.legend()
plt.xlabel('Time')
plt.ylabel('y(t)')
plt.title('Comparison of Observations and Fitted Model')
plt.savefig("fitted_model_comparison.png")
plt.show()