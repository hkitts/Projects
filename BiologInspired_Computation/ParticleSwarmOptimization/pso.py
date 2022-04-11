# CS 420/CS 527 Lab 5: Particle Swarm Optimization 
# Hunter Kitts
# April 2022

from nbformat import write
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import argparse
import decimal

class Particle:
    def __init__(self, position, function):
        self.position = np.array(position)
        self.v = np.array([0,0])
        self.best_position = self.position.copy()
        self.function = function
        
    def update(self, pso):
        self.v = pso.inertia*self.v+pso.phi_1*np.random.random(2)*(np.subtract(self.best_position,self.position))+pso.phi_2*np.random.random(2)*np.subtract(pso.global_best,self.position)
            
        if (self.v[0]**2+self.v[1]**2 > pso.max_vel**2):
            self.v = (pso.max_vel/np.sqrt(self.v[0]**2+self.v[1]**2))*self.v
        
        self.position = np.add(self.position, self.v)
            
        val = pso.Q(self.position, self.function)
        if (val < self.best_val):
            self.best_val = val
            self.best_position = self.position.copy()
        
        if (val < pso.global_best_val):
            pso.global_best_val = val
            pso.global_best = self.position.copy()
        
class PSO:
    #args.num_particles, args.inertia, args.cognition, args.social, 100, 100, 2)
    def __init__(self, num_particles, inertia, phi_1, phi_2, ww, wh, max_vel, function):
        self.num_particles = num_particles
        self.inertia = inertia
        self.phi_1 = phi_1
        self.phi_2 = phi_2
        self.ww = ww
        self.wh = wh
        self.max_vel = max_vel
        self.global_best = np.array([0,0])
        self.global_best_val = None
        self.particles = []
        self.function = function
        
        for i in range(num_particles):
            p = []
            p.append(np.random.random()*ww-ww/2)
            p.append(np.random.random()*wh-wh/2)
            particle = Particle(p, self.function)
            particle.best_val = self.Q(p, self.function)
            if (self.global_best_val == None or self.global_best_val > particle.best_val):
                self.global_best_val = particle.best_val
                self.global_best[:] = p[:]
            self.particles.append(particle)
            
    
    def Q(self, position, function):
        x = position[0]
        y = position[1]
        #Rosenbrock | Booth
        if function == "Rosenbrock":
            # Rosenbrock (banana) function
            val=(1-x)**2+100*(y-x**2)**2
        elif function == "Booth":
            # Booth function
            val = (x+2*y-7)**2 + (2*x+y-5)**2
        else:
            print("FUNCTION ERROR EXITING")
            exit()

        return val
    
    def update(self):
        for i in range(self.num_particles):
            p = self.particles[i]
            p.update(self)
            
    def scatter_plot(self):
        x = []
        y = []
        for i in range(self.num_particles):
            x.append(self.particles[i].position[0])
            y.append(self.particles[i].position[1])
        return x,y

parser = argparse.ArgumentParser(description="CS 420/CS 527 Lab 5: PSO")
parser.add_argument("--num_particles", default=40, type=int, help="Number of particles")
parser.add_argument("--inertia", default=0.5, type=float, help="Inertia")
parser.add_argument("--cognition", default=1, type=float, help="Cognition parameter")
parser.add_argument("--social", default=1, type=float, help="Social parameter")
parser.add_argument("--function", default="Rosenbrock", type=str, help="Rosenbrock | Booth")
parser.add_argument("--all", default=False, type=bool)
    
args = parser.parse_args()

if args.all:


    #args.num_particles = 10
    #args.inertia = 0.1
    #args.cognition = 0.1
    #args.social = 0.1
    args.function = "Rosenbrock"
    inertia_list = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0]
    cog_list = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0,
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0,]
    social_list = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8 , 0.9, 1.0, 
                1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 , 1.9, 2.0,
                2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8 , 2.9, 3.0,
                3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8 , 3.9, 4.0,]

    
    
    for func in range(2):
        if func == 1:
            args.function = "Booth"
        file_str = args.function + '_social.txt'
        file = open(file_str, 'w')
        for social in social_list:
            args.social = social
            for test in range(10):
        #    file_str = args.function + '_numPart_' + str(part) + '.txt'
        #    file = open(file_str, 'w')
        #    args.num_particles = part
        #    for inertia in inertia_list:
        #    args.inertia = inertia
        #    for cog in cog_list:
        #    args.cognition = cog
        #    for social in social_list:
        #    args.social = social

                # Print all of the command line arguments
                d = vars(args)
                for k in d.keys():
                    if k != "all":
                        file.write(k + str(":") + str(d[k]) + '\n')

                # Create PSO
                pso = PSO(args.num_particles, args.inertia, args.cognition, args.social, 100, 100, 2, args.function)

                for i in range(1000):
                    pso.update()
                    x,y = pso.scatter_plot()
                    error_x = np.sum([(pso.particles[k].position[0]-pso.global_best[0])**2 for k in range(args.num_particles)])
                    error_y = np.sum([(pso.particles[k].position[1]-pso.global_best[1])**2 for k in range(args.num_particles)])
                    error_x = np.sqrt((1.0/(2*args.num_particles))*error_x)
                    error_y = np.sqrt((1.0/(2*args.num_particles))*error_y)
                    if (error_x < 0.00001 and error_y < 0.00001):
                        break

                file.write("epoch_stop: " + str(i) + '\n')
                file.write("solution_found: " + str(pso.global_best)+ '\n')
                file.write("fitness: " + str(pso.global_best_val) + '\n')
                file.write('\n')
        file.close()

else:
    args = parser.parse_args()
    # Print all of the command line arguments
    d = vars(args)
    for k in d.keys():
        if k != "all":
            print(k + str(":") + str(d[k]))

    # Create PSO
    pso = PSO(args.num_particles, args.inertia, args.cognition, args.social, 100, 100, 2, args.function)

    for i in range(1000):
        pso.update()
        x,y = pso.scatter_plot()
        error_x = np.sum([(pso.particles[k].position[0]-pso.global_best[0])**2 for k in range(args.num_particles)])
        error_y = np.sum([(pso.particles[k].position[1]-pso.global_best[1])**2 for k in range(args.num_particles)])
        error_x = np.sqrt((1.0/(2*args.num_particles))*error_x)
        error_y = np.sqrt((1.0/(2*args.num_particles))*error_y)
        if (error_x < 0.00001 and error_y < 0.00001):
            break

    print("epoch_stop:", i)
    print("solution_found:", pso.global_best)
    print("fitness:", pso.global_best_val)
    print()
