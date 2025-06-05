import numpy as np
import random
import itertools
from scipy.spatial import distance_matrix
from scipy.sparse.csgraph import minimum_spanning_tree
import networkx as nx  # For finding minimum-weight perfect matching
import matplotlib.pyplot as plt
import csv


class TSPProblem:
    def __init__(self, filename):
        self.cities = self.parse_tsp_file(filename)
        self.dist_matrix = self.calculate_distance_matrix()

    def parse_tsp_file(self, filename):
        with open(filename, 'r') as file:
            lines = file.readlines()
        coords = []
        start_reading = False
        for line in lines:
            if line.strip() == "NODE_COORD_SECTION":
                start_reading = True
                continue
            if start_reading:
                if line.strip() == "EOF":
                    break
                parts = line.strip().split()
                x, y = float(parts[1]), float(parts[2])
                coords.append((x, y))
        return np.array(coords)

    def calculate_distance_matrix(self):
        return distance_matrix(self.cities, self.cities)

    @staticmethod
    def tour_length(tour, dist_matrix):
        return sum(dist_matrix[tour[i], tour[i + 1]] for i in range(len(tour) - 1)) + dist_matrix[tour[-1], tour[0]]


class TSPHeuristics:
    def __init__(self, dist_matrix):
        self.dist_matrix = dist_matrix

    def nearest_neighbour(self):
        num_cities = len(self.dist_matrix)
        start = random.randint(0, num_cities - 1)
        tour = [start]
        unvisited = set(range(num_cities)) - {start}

        while unvisited:
            last = tour[-1]
            next_city = min(unvisited, key=lambda x: self.dist_matrix[last, x])
            tour.append(next_city)
            unvisited.remove(next_city)

        return tour

    def cheapest_insertion(self):
        num_cities = len(self.dist_matrix)
        tour = [0, 1]
        unvisited = set(range(num_cities)) - set(tour)

        while unvisited:
            best_increase = float('inf')
            for u in unvisited:
                for i in range(len(tour)):
                    increase = (self.dist_matrix[tour[i - 1], u] +
                                self.dist_matrix[u, tour[i]] -
                                self.dist_matrix[tour[i - 1], tour[i]])
                    if increase < best_increase:
                        best_increase, best_city, best_position = increase, u, i
            tour.insert(best_position, best_city)
            unvisited.remove(best_city)

        return tour

    def farthest_insertion(self):
        num_cities = len(self.dist_matrix)
        tour = [0, np.argmax(self.dist_matrix[0])]
        unvisited = set(range(num_cities)) - set(tour)

        while unvisited:
            farthest_city = max(unvisited, key=lambda x: min(self.dist_matrix[x][i] for i in tour))
            best_position = None
            best_increase = float('inf')

            for i in range(len(tour)):
                increase = (self.dist_matrix[tour[i - 1], farthest_city] +
                            self.dist_matrix[farthest_city, tour[i]] -
                            self.dist_matrix[tour[i - 1], tour[i]])
                if increase < best_increase:
                    best_increase, best_position = increase, i
            tour.insert(best_position, farthest_city)
            unvisited.remove(farthest_city)

        return tour

    def mst_heuristic(self):
        mst = minimum_spanning_tree(self.dist_matrix).toarray().astype(float)
        tour = []
        visited = set()

        def dfs(v):
            visited.add(v)
            tour.append(v)
            for u in range(len(mst)):
                if mst[v, u] > 0 and u not in visited:
                    dfs(u)

        dfs(0)
        tour.append(tour[0])
        return tour

    def christofides(self):
        # Step 1: Compute the MST
        mst = minimum_spanning_tree(self.dist_matrix).toarray().astype(float)

        # Step 2: Find odd-degree vertices in MST
        degrees = np.sum(mst > 0, axis=1)
        odd_vertices = [i for i, deg in enumerate(degrees) if deg % 2 == 1]

        # Step 3: Find a minimum-weight perfect matching among odd-degree vertices
        odd_graph = nx.Graph()
        for i in odd_vertices:
            for j in odd_vertices:
                if i != j:
                    odd_graph.add_edge(i, j, weight=self.dist_matrix[i, j])

        # Compute minimum-weight matching
        matching = nx.algorithms.matching.min_weight_matching(odd_graph)

        # Step 4: Combine MST and matching to form a multigraph
        multigraph = nx.MultiGraph()
        for i, j in zip(*np.nonzero(mst)):
            multigraph.add_edge(i, j, weight=self.dist_matrix[i, j])
        for u, v in matching:
            multigraph.add_edge(u, v, weight=self.dist_matrix[u, v])

        # Ensure the graph is Eulerian
        degrees = [degree for _, degree in multigraph.degree()]
        if any(degree % 2 != 0 for degree in degrees):
            raise ValueError("Graph is not Eulerian after adding matching!")

        # Step 5: Find an Eulerian circuit in the multigraph
        eulerian_circuit = list(nx.eulerian_circuit(multigraph))

        # Step 6: Convert Eulerian circuit to Hamiltonian circuit by skipping repeated vertices
        tour = []
        visited = set()
        for u, v in eulerian_circuit:
            if u not in visited:
                tour.append(u)
                visited.add(u)

        # Return the Hamiltonian tour
        return tour

    def two_opt(self, tour):
        num_cities = len(tour)
        improvement = True
        while improvement:
            improvement = False
            for i in range(1, num_cities - 1):
                for j in range(i + 1, num_cities):
                    if j - i == 1:
                        continue
                    new_tour = tour[:i] + tour[i:j][::-1] + tour[j:]
                    if TSPProblem.tour_length(new_tour, self.dist_matrix) < TSPProblem.tour_length(tour, self.dist_matrix):
                        tour = new_tour
                        improvement = True
        return tour

    def three_opt(self, tour):
        num_cities = len(tour)
        for (i, j, k) in itertools.combinations(range(num_cities), 3):
            new_tour = min(
                [
                    tour[:i] + tour[i:j][::-1] + tour[j:k][::-1] + tour[k:],
                    tour[:i] + tour[i:j] + tour[j:k][::-1] + tour[k:],
                    tour[:i] + tour[j:k] + tour[i:j] + tour[k:]
                ],
                key=lambda t: TSPProblem.tour_length(t, self.dist_matrix)
            )
            if TSPProblem.tour_length(new_tour, self.dist_matrix) < TSPProblem.tour_length(tour, self.dist_matrix):
                tour = new_tour
        return tour

    def node_shift(self, tour):
        num_cities = len(tour)
        for i in range(num_cities):
            for j in range(num_cities):
                if i == j:
                    continue
                new_tour = tour[:i] + tour[i + 1:j] + [tour[i]] + tour[j:]
                if TSPProblem.tour_length(new_tour, self.dist_matrix) < TSPProblem.tour_length(tour, self.dist_matrix):
                    tour = new_tour
        return tour


class TSPRunner:
    def __init__(self, tsp_problem):
        self.problem = tsp_problem
        self.heuristics = TSPHeuristics(self.problem.dist_matrix)

    def run(self):
        # Constructive Heuristics
        initial_tours = {
            "Nearest Neighbour": self.heuristics.nearest_neighbour(),
            "Cheapest Insertion": self.heuristics.cheapest_insertion(),
            "Farthest Insertion": self.heuristics.farthest_insertion(),
            "MST Heuristic": self.heuristics.mst_heuristic(),
            "Christofides Heuristic": self.heuristics.christofides()
        }

        # Apply Perturbative Heuristics to each Constructive Heuristic
        perturbative_methods = {
            "2-opt": self.heuristics.two_opt,
            "3-opt": self.heuristics.three_opt,
            "Node Shift": self.heuristics.node_shift
        }

        tour_lengths = {}

        for constructive_name, initial_tour in initial_tours.items():
            # Save the initial constructive heuristic tour length
            initial_length = TSPProblem.tour_length(initial_tour, self.problem.dist_matrix)
            tour_lengths[f"{constructive_name} (Initial)"] = initial_length

            # Apply all perturbative methods
            for perturbative_name, perturbative_method in perturbative_methods.items():
                improved_tour = perturbative_method(initial_tour)
                improved_length = TSPProblem.tour_length(improved_tour, self.problem.dist_matrix)
                tour_lengths[f"{constructive_name} + {perturbative_name}"] = improved_length

        # Display results
        for heuristic, length in tour_lengths.items():
            print(f"{heuristic}: {length}")

        # Write results to a CSV file
        self.write_results_to_csv(tour_lengths)

        # Plotting the results
        self.plot_results(tour_lengths)

    @staticmethod
    def write_results_to_csv(tour_lengths, filename="tsp_results.csv"):
        with open(filename, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Heuristic", "Tour Length"])
            for heuristic, length in tour_lengths.items():
                writer.writerow([heuristic, length])
        print(f"Results saved to {filename}")

    @staticmethod
    def plot_results(tour_lengths):
        plt.figure(figsize=(14, 8))
        plt.bar(tour_lengths.keys(), tour_lengths.values(), color='skyblue')
        plt.xlabel("Heuristic")
        plt.ylabel("Tour Length")
        plt.title("Tour Lengths for Different Heuristics and Improvements")
        plt.xticks(rotation=90, ha='center', fontsize=8)
        plt.tight_layout()
        plt.show()


def main():
    filename = "a280.tsp"
    tsp_problem = TSPProblem(filename)
    tsp_runner = TSPRunner(tsp_problem)
    tsp_runner.run()


if __name__ == "__main__":
    main()
