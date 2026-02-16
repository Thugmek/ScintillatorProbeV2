def get_area(samples):
    len_samples = len(samples)
    avg = 0
    for i in range(len_samples-1):
        avg += (samples[i]+samples[i+1])/2
    avg /= len_samples
    return avg