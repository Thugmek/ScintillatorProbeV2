import matplotlib.pyplot as plt

class Line:
    def __init__(self, x, y, slope):
        self.x = x
        self.y = y
        self.slope = slope

    def get_y(self, x):
        return (x-self.x)*self.slope + self.y

    def plot(self, p, xses):
        yses = [
            self.get_y(xses[0]),
            self.get_y(xses[1])
        ]

        p.plot(xses, yses)

    def intersection(self, l2):
        x = (self.slope*self.x - l2.slope*l2.x + l2.y - self.y)/(self.slope - l2.slope)
        y = self.slope*(x - self.x) + self.y
        return (x, y)