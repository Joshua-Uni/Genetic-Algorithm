# Genetic Algorithm:
Welcome to my Genetic Algorithm project! This C programming language-based project showcases the power of genetic algorithms in simulating the evolution of a flock of creatures within a predefined environment. By leveraging genetic principles, the flock gradually learns to navigate the environment more efficiently and effectively towards a given target.


<img width="500" alt="before" src="https://github.com/Joshua-Uni/Genetic-Algorithm/assets/112139913/e121b6cf-5a95-44c7-8093-d261b3d455bc">

<img width="500" alt="after" src="https://github.com/Joshua-Uni/Genetic-Algorithm/assets/112139913/5237341f-a93e-41d8-ad01-aec2561b2828">

Left: 1st generation V.S. Right: 10th generation




# Flock Initialization and DNA

At the beginning of the simulation, a diverse flock of creatures is generated. Each creature possesses a unique DNA that determines various characteristics, such as their movement pattern, size, color, and initial spawn location. Additionally, a target is placed within the environment as the objective for the creatures.

# Survival Score and Selection

Throughout the simulation, the survival score of each creature is calculated based on their proximity to the target. The closer a creature gets to the target, the higher its survival score. This scoring mechanism emulates the natural selection process, where creatures with better survival scores are more likely to pass on their genetic traits to the next generation.

# Generation Transition and Population Transfer

Once a round of movement is complete, a new flock is generated. Initially, some creatures from the previous generation are randomly selected to populate the new flock. However, creatures with higher survival scores have a significantly higher chance of being chosen. This process mimics the concept of "survival of the fittest" in nature.

# DNA Mutation and Variation

Following the population transfer, the DNA of the new flock undergoes random mutations. This mutation includes modifications to movement patterns, starting positions, sizes, and colors. By introducing these random variations, the algorithm explores different possibilities and encourages the emergence of more successful traits in subsequent generations.

# Evolution and Optimization

As the genetic algorithm continues to iterate over time, observable changes occur within the flock. Initially, the creatures exhibit a wide range of characteristics with no apparent similarities. However, as generations progress, the flock gradually evolves, resulting in creatures that exhibit more similarities in terms of color and size. Consequently, the evolved flock demonstrates improved navigation capabilities within the environment, as they have effectively learned to adapt and optimize their behavior.

# Educational Value and Exploration

The Genetic Algorithm for Flock Behavior Optimization project serves as a valuable educational resource and practical demonstration of genetic algorithms in action. By exploring and manipulating the various parameters and DNA attributes, users can observe firsthand how genetic algorithms can drive the evolution of a population towards an optimal solution. Whether you are a student, researcher, or enthusiast, this project provides a fascinating insight into the potential of genetic algorithms in solving complex optimization problems.

Get ready to witness the captivating journey of a flock of creatures as they evolve and adapt to their environment through the power of genetic algorithms!

# NOTE:
To compile this project requires SDL which can be difficult to install and use. This tutorial was very helpful and i use the comprun.sh file from it to compile and run my project. https://www.youtube.com/watch?v=LXzASmnAcXA
