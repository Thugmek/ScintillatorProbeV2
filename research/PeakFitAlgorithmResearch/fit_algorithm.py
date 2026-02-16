import math

class Model:
    def __init__(self):
        self.params = 0
        self.start_params = []
        self.name = "Dummy"

    def get_val(self, x, params):
        return 0

    def get_error_for_try(self, samples, params):
        error = 0
        for i in range(len(samples)):
            error +=((samples[i] - self.get_val(i, params)) ** 2) * samples[i]
        return error

class Landau(Model):
    def __init__(self):
        super().__init__()
        self.params = 3
        self.start_params = [1,1,0] #[vscale, hscale, hoffset]
        self.name = "Landau"

    def get_val(self, x, params):
        # params = [vscale, hscale, hoffset]
        x_scaled = (x - params[2]) * params[1]
        if x_scaled > 0:
            return (math.pow(math.e, -1 / x_scaled) / (x_scaled * (x_scaled + 1))) * params[0]
        else:
            return 0

class ParabolicExponential(Model):
    def __init__(self):
        super().__init__()
        self.params = 3
        self.start_params = [1,1,0.9,20] #[vscale, hscale, decay, hoffset]
        self.name = "Parabolic-exponential"

    def get_val(self, x, params):
        # params = [vscale, hscale, decay, hoffset]
        x_scaled = (x - params[3] - 1) * params[1]
        if x_scaled > 0:
            #logarithmic
            if params[2] <= 0:
                return 0
            return math.pow(params[2],x_scaled)*params[0]
        elif x_scaled > -1:
            return (1 - x_scaled**2)*params[0]
        else:
            return 0

class TwoExp(Model):
    def __init__(self):
        super().__init__()
        self.params = 4
        self.start_params = [1.3,0.5,5.5,8] #[scale, t_rise, t_decay, h_offset]
        self.name = "Two exponentials"

    def get_val(self, x, params):
        # params = [scale, t_rise, t_decay, h_offset]
        x_scaled = x - params[3]
        if x_scaled > 0:
            exp1 = math.pow(math.e, -x_scaled/params[2])
            exp2 = math.pow(math.e, -x_scaled / params[1])
            return params[0] * (exp1-exp2)
        else:
            return 0

class Levy(Model):
    def __init__(self):
        super().__init__()
        self.params = 3
        self.start_params = [1, 1, 0]  # [v_scale, h_scale, h_offset]
        self.name = "Lévy"

    def get_val(self, x, params):
        # params = [scale, c, h_offset]
        x_scaled = (x - params[2])*params[1]
        if x_scaled > 0:
            num = math.pow(math.e, -1/(2*x_scaled))
            denom = math.pow(x_scaled,3/2)
            return (num/denom)*params[0]
        else:
            return 0

def fit_function(samples, model:Model):
    params = model.start_params[:]
    step = 0.1
    last_error = float('inf')
    is_better_for = 0
    for i in range(10000):
        is_better = False
        for j in range(len(params)):
            next_param = params[:]
            next_param[j] = next_param[j] + step
            e = model.get_error_for_try(samples, next_param)
            if e < last_error:
                last_error = e
                params[j] = params[j] + step
                is_better = True

            next_param = params[:]
            next_param[j] = next_param[j] - step
            e = model.get_error_for_try(samples, next_param)
            if e < last_error:
                last_error = e
                params[j] = params[j] - step
                is_better = True

        if not is_better:
            step /= 2
            if step == 0:
                break
        else:
            is_better_for += 1
            if is_better_for > 5:
                is_better_for = 0
                step *= 2

    print(f"params = {params}, step = {step}")

    def f(x):
        return model.get_val(x, params)
    return f
