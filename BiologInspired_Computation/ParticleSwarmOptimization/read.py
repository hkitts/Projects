from matplotlib import markers
import numpy as np 
import matplotlib.pyplot as plt
import os
import sys
from pathlib import Path

class Data:
    def __init__(self):
        self.Booth = {
            'C': {'particles': [], 'inertia': [], 'cognition': [], 'social': []},
            'N': {'particles': [], 'inertia': [], 'cognition': [], 'social': []}
            }
        self.Rosenbrock = {
            'C': {'particles': [], 'inertia': [], 'cognition': [], 'social': []},
            'N': {'particles': [], 'inertia': [], 'cognition': [], 'social': []}
            }
        self.ten_runs = {0: [], 1: [], 2: [], 3: [], 4: [], 5: [], 6: [], 7: [], 8: [], 9: []}
        self.particles = {10: [], 20: [], 30: [], 40: [], 50: [], 60: [], 70: [], 80: [], 90: [], 100: []}
        self.inertia = {0.1: [], 0.2: [], 0.3: [], 0.4: [], 0.5: [], 0.6: [], 0.7: [], 0.8: [], 0.9: [], 1.0: []}
        self.cognition = {0.1: [], 0.2: [], 0.3: [], 0.4: [], 0.5: [], 0.6: [], 0.7: [], 0.8 : [], 0.9: [], 1.0: [],
                          1.1: [], 1.2: [], 1.3: [], 1.4: [], 1.5: [], 1.6: [], 1.7: [], 1.8 : [], 1.9: [], 2.0: [],
                          2.1: [], 2.2: [], 2.3: [], 2.4: [], 2.5: [], 2.6: [], 2.7: [], 2.8 : [], 2.9: [], 3.0: [],
                          3.1: [], 3.2: [], 3.3: [], 3.4: [], 3.5: [], 3.6: [], 3.7: [], 3.8 : [], 3.9: [], 4.0: []}
        self.social =    {0.1: [], 0.2: [], 0.3: [], 0.4: [], 0.5: [], 0.6: [], 0.7: [], 0.8 : [], 0.9: [], 1.0: [],
                          1.1: [], 1.2: [], 1.3: [], 1.4: [], 1.5: [], 1.6: [], 1.7: [], 1.8 : [], 1.9: [], 2.0: [],
                          2.1: [], 2.2: [], 2.3: [], 2.4: [], 2.5: [], 2.6: [], 2.7: [], 2.8 : [], 2.9: [], 3.0: [],
                          3.1: [], 3.2: [], 3.3: [], 3.4: [], 3.5: [], 3.6: [], 3.7: [], 3.8 : [], 3.9: [], 4.0: []}
        self.particles_line = []
        self.inertia_line = []
        self.cognition_line = []
        self.social_line = []

    def reset_dicts(self):
        self.particles = {10: [], 20: [], 30: [], 40: [], 50: [], 60: [], 70: [], 80: [], 90: [], 100: []}
        self.inertia = {0.1: [], 0.2: [], 0.3: [], 0.4: [], 0.5: [], 0.6: [], 0.7: [], 0.8: [], 0.9: [], 1.0: []}
        self.cognition = {0.1: [], 0.2: [], 0.3: [], 0.4: [], 0.5: [], 0.6: [], 0.7: [], 0.8 : [], 0.9: [], 1.0: [],
                          1.1: [], 1.2: [], 1.3: [], 1.4: [], 1.5: [], 1.6: [], 1.7: [], 1.8 : [], 1.9: [], 2.0: [],
                          2.1: [], 2.2: [], 2.3: [], 2.4: [], 2.5: [], 2.6: [], 2.7: [], 2.8 : [], 2.9: [], 3.0: [],
                          3.1: [], 3.2: [], 3.3: [], 3.4: [], 3.5: [], 3.6: [], 3.7: [], 3.8 : [], 3.9: [], 4.0: []}
        self.social =    {0.1: [], 0.2: [], 0.3: [], 0.4: [], 0.5: [], 0.6: [], 0.7: [], 0.8 : [], 0.9: [], 1.0: [],
                          1.1: [], 1.2: [], 1.3: [], 1.4: [], 1.5: [], 1.6: [], 1.7: [], 1.8 : [], 1.9: [], 2.0: [],
                          2.1: [], 2.2: [], 2.3: [], 2.4: [], 2.5: [], 2.6: [], 2.7: [], 2.8 : [], 2.9: [], 3.0: [],
                          3.1: [], 3.2: [], 3.3: [], 3.4: [], 3.5: [], 3.6: [], 3.7: [], 3.8 : [], 3.9: [], 4.0: []}
        self.particles_line = []
        self.inertia_line = []
        self.cognition_line = []
        self.social_line = []

    def test_converge(self, fitness):
        if fitness > 1e-10:
            return 'N'
        else:
            return 'C'

    def get_int(self, value):
        value = str(value)
        value = value.split(':')
        return int(value[1])

    def get_float(self, value):
        value = str(value)
        value = value.split(':')
        return float(value[1])

    def get_function(self, value):
        value = str(value)
        value = value.split(':')
        return value[1]

    def get_solution(self, solution):
        solution = str(solution)
        solution = solution.split(':')
        solution = solution[1].strip(']')
        solution = solution[2:]
        solution = solution.strip()
        solution = solution.split(' ')
        #print(solution)
        x = float(solution[0])
        for i in range(len(solution)):
            if i != 0 and solution[i] != '':
                y = float(solution[i])
                break
        solution = [x, y]
        return solution

    def process_test(self, test, i, parameter):
        particles = self.get_int(test[0])
        inertia = self.get_float(test[1])
        cognition = self.get_float(test[2])
        social = self.get_float(test[3])
        function = self.get_function(test[4])
        epoch_stop = self.get_int(test[5])
        solution = self.get_solution(test[6])
        fitness = self.get_float(test[7])
        convergance = self.test_converge(fitness)        
        test = [particles, inertia, cognition, social, epoch_stop, solution, fitness]
        
        if function == "Booth":
            #insert into self.Booth
            if convergance == 'C':
                #insert into self.Booth C
                self.Booth['C'][parameter].append(test)
                j = 1
            elif convergance == 'N':
                #insert into non convergance 
                self.Booth['N'][parameter].append(test)
            else: 
                print('CONVERGANCE ERROR')
                exit()
        elif function == "Rosenbrock":
            #insert into self.Rosenbrock
            if convergance == 'C':
                #insert into self.Rosenbrock C
                self.Rosenbrock['C'][parameter].append(test)
            elif convergance == 'N':
                #insert into non convergance 
                self.Rosenbrock['N'][parameter].append(test)
            else: 
                print('CONVERGANCE ERROR')
                exit()
        else:
            print('FUNCTION ERROR')
            exit()
        
    def print_booth(self):
        print('Convergance')
        convergance = self.Booth['C']
        for key, value in convergance.items():
            print(key + ':')
            test_list = value
            for i in test_list:
                test = i
                for j in range(len(test)):
                    print('\t' + str(test[j]))
                print()

#########################################################################
## functions for converged plots  ##
    def store_particles_booth_c(self):
        convergance = self.Booth['C']
        for key, value in convergance.items():
            if key == 'particles':
                test_list = value
                for i in test_list:
                    test = i
                    self.particles[test[0]].append(test[4])

    def plot_particles_booth_c(self):
        particles = []
        for key, value in self.particles.items():
            y = np.asarray(value)
            mean = np.mean(y)
            particles.append(y)
            self.particles_line.append(mean)
        x = np.asarray(range(1, 11))
        plt.plot(x, self.particles_line, marker='o')
        plt.boxplot(particles, labels= ['10', '20', '30', '40', '50', '60', '70', '80', '90', '100'])
        plt.title('Booth - Converged - Epoch vs Number of Particles')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Number of Particles')
        plt.savefig("Booth_C_num_particles.jpg")
        plt.close()

    def store_inertia_booth_c(self):
        convergance = self.Booth['C']
        for key, value in convergance.items():
            if key == 'inertia':
                test_list = value
                for i in test_list:
                    test = i
                    self.inertia[test[1]].append(test[4])

    def plot_inertia_booth_c(self):
        inertia = []
        for key, value in self.inertia.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.inertia_line.append(mean)
            else:
                self.inertia_line.append(0)
            inertia.append(y)
            
        x = np.asarray(range(1, 11))
        plt.plot(x, self.inertia_line, marker='o')
        plt.boxplot(inertia, labels= ['0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0'])
        plt.title('Booth - Converged - Epoch vs Inertia')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Inertia')
        plt.savefig("Booth_C_inertia.jpg")
        plt.close()

    def store_cognition_booth_c(self):
        convergance = self.Booth['C']
        for key, value in convergance.items():
            if key == 'cognition':
                test_list = value
                for i in test_list:
                    test = i
                    self.cognition[test[2]].append(test[4])

    def plot_cognition_booth_c(self):
        cognition = []
        for key, value in self.cognition.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.cognition_line.append(mean)
            else:
                self.cognition_line.append(0)
            cognition.append(y)

        x = np.asarray(range(1, 41))
        plt.figure(figsize=(14, 6))
        plt.plot(x, self.cognition_line, marker='o')
        plt.boxplot(cognition, labels= [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.title('Booth - Converged - Epoch vs Cognition')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Cognition')
        plt.savefig("Booth_C_cognition.jpg")
        plt.close()

    def store_social_booth_c(self):
        convergance = self.Booth['C']
        for key, value in convergance.items():
            if key == 'social':
                test_list = value
                for i in test_list:
                    test = i
                    self.social[test[3]].append(test[4])

    def plot_social_booth_c(self):
        social = []
        for key, value in self.social.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.social_line.append(mean)
            else:
                self.social_line.append(0)
            social.append(y)

        x = np.asarray(range(1, 41))
        plt.figure(figsize=(14, 6))
        plt.plot(x, self.social_line, marker='o')
        plt.boxplot(social, labels= [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.title('Booth - Converged - Epoch vs Social')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Social')
        plt.savefig("Booth_C_social.jpg")
        plt.close()
    
    def store_particles_rosenbrock_c(self):
        convergance = self.Rosenbrock['C']
        for key, value in convergance.items():
            if key == 'particles':
                test_list = value
                for i in test_list:
                    test = i
                    self.particles[test[0]].append(test[4])

    def plot_particles_rosenbrock_c(self):
        particles = []
        for key, value in self.particles.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.particles_line.append(mean)
            else:
                self.particles_line.append(0)
            particles.append(y)
        x = np.asarray(range(1, 11))
        plt.plot(x, self.particles_line, marker='o')
        plt.boxplot(particles, labels= ['10', '20', '30', '40', '50', '60', '70', '80', '90', '100'])
        plt.title('Rosenbrock - Converged - Epoch vs Number of Particles')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Number of Particles')
        plt.savefig("Rosenbrock_C_num_particles.jpg")
        plt.close()

    def store_inertia_rosenbrock_c(self):
        convergance = self.Rosenbrock['C']
        for key, value in convergance.items():
            if key == 'inertia':
                test_list = value
                for i in test_list:
                    test = i
                    self.inertia[test[1]].append(test[4])

    def plot_inertia_rosenbrock_c(self):
        inertia = []
        for key, value in self.inertia.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.inertia_line.append(mean)
            else:
                self.inertia_line.append(0)
            inertia.append(y)
        x = np.asarray(range(1, 11))
        plt.plot(x, self.inertia_line, marker='o')
        plt.boxplot(inertia, labels= ['0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0'])
        plt.title('Rosenbrock - Converged - Epoch vs Inertia')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Inertia')
        plt.savefig("Rosenbrock_C_inertia.jpg")
        plt.close()

    def store_cognition_rosenbrock_c(self):
        convergance = self.Rosenbrock['C']
        for key, value in convergance.items():
            if key == 'cognition':
                test_list = value
                for i in test_list:
                    test = i
                    self.cognition[test[2]].append(test[4])

    def plot_cognition_rosenbrock_c(self):
        cognition = []
        for key, value in self.cognition.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.cognition_line.append(mean)
            else:
                self.cognition_line.append(0)
            cognition.append(y)
        
        x = np.asarray(range(1, 41))
        plt.figure(figsize=(14, 6))
        plt.plot(x, self.cognition_line, marker='o')
        plt.boxplot(cognition, labels= [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.title('Rosenbrock - Converged - Epoch vs Cognition')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Cognition')
        plt.savefig("Rosenbrock_C_cognition.jpg")
        plt.close()

    def store_social_rosenbrock_c(self):
        convergance = self.Rosenbrock['C']
        for key, value in convergance.items():
            if key == 'social':
                test_list = value
                for i in test_list:
                    test = i
                    self.social[test[3]].append(test[4])

    def plot_social_rosenbrock_c(self):
        social = []
        for key, value in self.social.items():
            y = np.asarray(value)
            if len(y) != 0:
                mean = np.mean(y)
                self.social_line.append(mean)
            else:
                self.social_line.append(0)
            social.append(y)
        
        x = np.asarray(range(1, 41))
        plt.figure(figsize=(14, 6))
        plt.plot(x, self.social_line, marker='o')
        plt.boxplot(social, labels= [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.title('Rosenbrock - Converged - Epoch vs Social')
        plt.ylabel('Epoch to Converge')
        plt.xlabel('Social')
        plt.savefig("Rosenbrock_C_social.jpg")
        plt.close()


###############################################################
#functions for non converged
    def store_particles_booth_n(self):
        convergance = self.Booth['N']
        for key, value in convergance.items():
            if key == 'particles':
                test_list = value
                for i in test_list:
                    test = i
                    self.particles[test[0]].append(test[4])

    def plot_particles_booth_n(self):
        particles = []
        for key, value in self.particles.items():
            y = len(value)
            particles.append(y)
        plt.bar(['10', '20', '30', '40', '50', '60', '70', '80', '90', '100'], particles)
        plt.yticks(range(1, 10))
        plt.title('Booth - Number of Nonconvergent - Number of Particles')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Number of Particles')
        plt.savefig("Booth_N_num_particles.jpg")
        plt.close()

    def store_inertia_booth_n(self):
        convergance = self.Booth['N']
        for key, value in convergance.items():
            if key == 'inertia':
                test_list = value
                for i in test_list:
                    test = i
                    self.inertia[test[1]].append(test[4])

    def plot_inertia_booth_n(self):
        inertia = []
        for key, value in self.inertia.items():
            y = len(value)
            inertia.append(y)
        plt.bar(['0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0'], inertia)
        plt.yticks(range(1, 10))
        plt.title('Booth - Number of Nonconvergent - Inertia')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Inertia')
        plt.savefig("Booth_N_inertia.jpg")
        plt.close()

    def store_cognition_booth_n(self):
        convergance = self.Booth['N']
        for key, value in convergance.items():
            if key == 'cognition':
                test_list = value
                for i in test_list:
                    test = i
                    self.cognition[test[2]].append(test[4])

    def plot_cognition_booth_n(self):
        cognition = []
        for key, value in self.cognition.items():
            y = len(value)
            cognition.append(y)
        plt.figure(figsize=(14, 6))
        plt.yticks(range(1, 10))
        plt.xticks([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.bar([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0], cognition, width=0.07)
        plt.title('Booth - Number of Nonconvergent - Cognition')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Cognition')
        plt.savefig("Booth_N_cognition.jpg")
        plt.close()

    def store_social_booth_n(self):
        convergance = self.Booth['N']
        for key, value in convergance.items():
            if key == 'social':
                test_list = value
                for i in test_list:
                    test = i
                    self.social[test[3]].append(test[4])

    def plot_social_booth_n(self):
        social = []
        for key, value in self.social.items():
            y = len(value)
            social.append(y)
        #print(social)
        plt.figure(figsize=(14, 6))
        plt.yticks(range(1, 10))
        plt.xticks([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.bar([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0], social, width=0.07)
        plt.title('Booth - Number of Nonconvergent - Social')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Social')
        plt.savefig("Booth_N_social.jpg")
        plt.close()
    
    def store_particles_rosenbrock_n(self):
        convergance = self.Rosenbrock['N']
        for key, value in convergance.items():
            if key == 'particles':
                test_list = value
                for i in test_list:
                    test = i
                    self.particles[test[0]].append(test[4])

    def plot_particles_rosenbrock_n(self):
        particles = []
        for key, value in self.particles.items():
            y = len(value)
            particles.append(y)
        plt.bar(['10', '20', '30', '40', '50', '60', '70', '80', '90', '100'], particles)
        plt.yticks(range(1, 10))
        plt.title('Rosenbrock - Number of Nonconvergent - Number of Particles')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Number of Particles')
        plt.savefig("Rosenbrock_N_num_particles.jpg")
        plt.close()

    def store_inertia_rosenbrock_n(self):
        convergance = self.Rosenbrock['N']
        for key, value in convergance.items():
            if key == 'inertia':
                test_list = value
                for i in test_list:
                    test = i
                    self.inertia[test[1]].append(test[4])

    def plot_inertia_rosenbrock_n(self):
        inertia = []
        for key, value in self.inertia.items():
            y = len(value)
            inertia.append(y)
        plt.bar(['0.1', '0.2', '0.3', '0.4', '0.5', '0.6', '0.7', '0.8', '0.9', '1.0'], inertia)
        plt.yticks(range(1, 10))
        plt.title('Rosenbrock - Number of Nonconvergent - Inertia')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Inertia')
        plt.savefig("Rosenbrock_N_inertia.jpg")
        plt.close()

    def store_cognition_rosenbrock_n(self):
        convergance = self.Rosenbrock['N']
        for key, value in convergance.items():
            if key == 'cognition':
                test_list = value
                for i in test_list:
                    test = i
                    self.cognition[test[2]].append(test[4])

    def plot_cognition_rosenbrock_n(self):
        cognition = []
        for key, value in self.cognition.items():
            y = len(value)
            cognition.append(y)
        plt.figure(figsize=(14, 6))
        plt.yticks(range(1,10))
        plt.xticks([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.bar([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0], cognition, width=0.07)
        plt.title('Rosenbrock - Number of Nonconvergent - Cognition')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Cognition')
        plt.savefig("Rosenbrock_N_cognition.jpg")
        plt.close()

    def store_social_rosenbrock_n(self):
        convergance = self.Rosenbrock['N']
        for key, value in convergance.items():
            if key == 'social':
                test_list = value
                for i in test_list:
                    test = i
                    self.social[test[3]].append(test[4])

    def plot_social_rosenbrock_n(self):
        social = []
        for key, value in self.social.items():
            y = len(value)
            social.append(y)
        plt.figure(figsize=(14, 6))
        plt.yticks(range(1,10))
        plt.xticks([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0])
        plt.bar([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0], social, width=0.07)
        plt.title('Rosenbrock - Number of Nonconvergent - Social')
        plt.ylabel('Number of Nonconvergent Tests')
        plt.xlabel('Social')
        plt.savefig("Rosenbrock_N_social.jpg")
        plt.close()

    
    def booth_converged(self):
        self.store_particles_booth_c()
        self.plot_particles_booth_c()
        self.store_inertia_booth_c()
        self.plot_inertia_booth_c()
        self.store_cognition_booth_c()
        self.plot_cognition_booth_c()
        self.store_social_booth_c()
        self.plot_social_booth_c()
        self.reset_dicts()
    
    def rosenbrock_converged(self):
        self.store_particles_rosenbrock_c()
        self.plot_particles_rosenbrock_c()
        self.store_inertia_rosenbrock_c()
        self.plot_inertia_rosenbrock_c()
        self.store_cognition_rosenbrock_c()
        self.plot_cognition_rosenbrock_c()
        self.store_social_rosenbrock_c()
        self.plot_social_rosenbrock_c()
        self.reset_dicts()

    def booth_nonconvergent(self):
        self.store_particles_booth_n()
        self.plot_particles_booth_n()
        self.store_inertia_booth_n()
        self.plot_inertia_booth_n()
        self.store_cognition_booth_n()
        self.plot_cognition_booth_n()
        self.store_social_booth_n()
        self.plot_social_booth_n()
        self.reset_dicts()

    def rosenbrock_nonconvergent(self):
        self.store_particles_rosenbrock_n()
        self.plot_particles_rosenbrock_n()
        self.store_inertia_rosenbrock_n()
        self.plot_inertia_rosenbrock_n()
        self.store_cognition_rosenbrock_n()
        self.plot_cognition_rosenbrock_n()
        self.store_social_rosenbrock_n()
        self.plot_social_rosenbrock_n()
        self.reset_dicts()

if __name__ == '__main__':
    data_directory = Path('Runs/')
    files = data_directory.iterdir()
    fileData = Data()

    for file in files:
        file_name = str(file)
        file_name = file_name.strip('.txt')
        file_name = file_name[5:]
        file_name = file_name.split('_')
        parameter = file_name[1]

        with file.open('r') as f:
            test = []
            i = 0
            for line in f:
                test.append(line.strip())
                if len(test) == 9:
                    fileData.process_test(test, i, parameter)
                    i += 1
                    test = []
                    if(i > 9): i = 0

    #Booth converge graphs
    fileData.booth_converged()
    fileData.rosenbrock_converged()
    fileData.booth_nonconvergent()
    fileData.rosenbrock_nonconvergent()

    