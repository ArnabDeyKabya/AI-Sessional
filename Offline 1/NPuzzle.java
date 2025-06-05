import java.util.*;

class Node {
    int[][] puzzle;
    int actualCost;
    int totalCost = 100000;
    int star;
    int previous;
    Node parent;

    public Node(int k) {
        puzzle = new int[k][k];
    }
}

class Compare implements Comparator<Node> {
    public int compare(Node a, Node b) {
        return a.totalCost - b.totalCost;
    }
}

public class NPuzzle {
    static int k; // size of the puzzle
    static int totalMoves = 0;

    public static int hamming(int[][] puzzle) {
        int count = 0;
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                if (puzzle[i][j] != 0 && puzzle[i][j] != i * k + j + 1) {
                    count++;
                }
            }
        }
        return count;
    }

    public static int manhattan(int[][] puzzle) {
        int count = 0;
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                if (puzzle[i][j] != 0) {
                    count += Math.abs(i - (puzzle[i][j] - 1) / k) + Math.abs(j - (puzzle[i][j] - 1) % k);
                }
            }
        }
        return count;
    }

    public static int inversion(int[][] puzzle) {
        int count = 0;
        int size = k * k - 1;
        int[] arr = new int[size];
        int index = 0;
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                if (puzzle[i][j] != 0) {
                    arr[index++] = puzzle[i][j];
                }
            }
        }
        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                if (arr[i] != 0 && arr[j] != 0 && arr[i] > arr[j]) {
                    count++;
                }
            }
        }
        return count;
    }

    public static void path(Node n) {
        if (n.parent != null) {
            path(n.parent);
        }
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                System.out.print(n.puzzle[i][j] + " ");
            }
            System.out.println();
        }
        totalMoves++;
        System.out.println();
    }

    public static boolean isSolvable(Node n) {
        boolean case1 = (k % 2 == 0 && (k - (n.star - 1) / k) % 2 == 0 && inversion(n.puzzle) % 2 == 1);
        boolean case2 = (k % 2 == 0 && (k - (n.star - 1) / k) % 2 == 1 && inversion(n.puzzle) % 2 == 0);
        boolean case3 = (k % 2 == 1 && inversion(n.puzzle) % 2 == 0);
        return case1 || case2 || case3;
    }

    public static void manhattanCost(PriorityQueue<Node> open) {
        int expanded = 0;
        int explored = 1;
        int[] h = {0, 0, 1, -1};
        int[] v = {1, -1, 0, 0};

        while (!open.isEmpty()) {
            Node current = open.poll();
            if (manhattan(current.puzzle) != 0) {
                expanded++;
                int r = (current.star - 1) / k;
                int c = (current.star - 1) % k;
                for (int i = 0; i < 4; i++) {
                    if (r + h[i] >= 0 && r + h[i] < k && c + v[i] >= 0 && c + v[i] < k &&
                            (r + h[i]) * k + c + v[i] + 1 != current.previous) {
                        Node child = new Node(k);
                        for (int p = 0; p < k; p++) {
                            for (int q = 0; q < k; q++) {
                                child.puzzle[p][q] = current.puzzle[p][q];
                            }
                        }
                        swap(child.puzzle, r + h[i], c + v[i], r, c);
                        child.actualCost = current.actualCost + 1;
                        child.star = (r + h[i]) * k + c + v[i] + 1;
                        child.totalCost = child.actualCost + manhattan(child.puzzle);
                        child.previous = current.star;
                        child.parent = current;
                        open.add(child);
                        explored++;
                    }
                }
            } else {
                System.out.println("The puzzle is solved");
                System.out.println("The path is: ");
                path(current);
                System.out.println("Minimum number of moves: " + (totalMoves - 1));
                System.out.println("For Manhattan:");
                System.out.println("The number of nodes expanded: " + expanded);
                System.out.println("The number of nodes explored: " + explored);
                return;
            }
        }
    }

    public static void hammingCost(PriorityQueue<Node> open) {
        int expanded = 0;
        int explored = 1;
        int[] h = {0, 0, 1, -1};
        int[] v = {1, -1, 0, 0};

        while (!open.isEmpty()) {
            Node current = open.poll();
            if (hamming(current.puzzle) != 0) {
                expanded++;
                int r = (current.star - 1) / k;
                int c = (current.star - 1) % k;
                for (int i = 0; i < 4; i++) {
                    if (r + h[i] >= 0 && r + h[i] < k && c + v[i] >= 0 && c + v[i] < k &&
                            (r + h[i]) * k + c + v[i] + 1 != current.previous) {
                        Node child = new Node(k);
                        for (int p = 0; p < k; p++) {
                            for (int q = 0; q < k; q++) {
                                child.puzzle[p][q] = current.puzzle[p][q];
                            }
                        }
                        swap(child.puzzle, r + h[i], c + v[i], r, c);
                        child.actualCost = current.actualCost + 1;
                        child.star = (r + h[i]) * k + c + v[i] + 1;
                        child.totalCost = child.actualCost + hamming(child.puzzle);
                        child.previous = current.star;
                        child.parent = current;
                        open.add(child);
                        explored++;
                    }
                }
            } else {
                System.out.println("The puzzle is solved");
                System.out.println("The path is: ");
                path(current);
                System.out.println("Minimum number of moves: " + (totalMoves - 1));
                System.out.println("For Hamming:");
                System.out.println("The number of nodes expanded: " + expanded);
                System.out.println("The number of nodes explored: " + explored);
                return;
            }
        }
    }

    public static void swap(int[][] puzzle, int r1, int c1, int r2, int c2) {
        int temp = puzzle[r1][c1];
        puzzle[r1][c1] = puzzle[r2][c2];
        puzzle[r2][c2] = temp;
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.print("Enter the size of the puzzle: ");
        k = sc.nextInt();

        Node start = new Node(k);
        System.out.println("Enter the start state of the puzzle: ");
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < k; j++) {
                start.puzzle[i][j] = sc.nextInt();
                if (start.puzzle[i][j] == 0) {
                    start.star = i * k + j + 1;
                }
            }
        }

        if (!isSolvable(start)) {
            System.out.println("The puzzle is not solvable");
            return;
        }

        PriorityQueue<Node> open = new PriorityQueue<>(new Compare());
        start.actualCost = 0;
        start.totalCost = 0;
        start.previous = 0;
        start.parent = null;
        open.add(start);

        System.out.println("Enter 1 for Manhattan and 2 for Hamming:");
        int choice = sc.nextInt();
        if (choice == 1) {
            manhattanCost(open);
        } else {
            hammingCost(open);
        }
    }
}
