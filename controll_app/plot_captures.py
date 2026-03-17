import matplotlib.pyplot as plt

for i in range(30):
    with open(f"capture_{i}") as f:
        r = f.read()[1:-1].split(",")
        capture = [int(c.strip()) for c in r]
        plt.plot(capture)

plt.show();