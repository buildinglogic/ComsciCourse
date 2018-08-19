import java.io.File;
import java.util.Arrays;
import java.util.Scanner;

public class Benchmark {
	
	private static final int trials = 30;
	
	private static TextGenerator getGenerator() {
//		return new BruteGenerator();
		return new MapGenerator();
	}
	
	private static double[] benchmark(int textLength, int k, Scanner source) throws Exception {
		TextGenerator generator = getGenerator();
		generator.train(source, "", k);
		double[] times = new double[trials];
		for (int i = 0; i < trials; i++) {
			Thread thread = new Thread(() -> {
				generator.generateText(textLength);
			});
			double start = System.nanoTime();
			thread.run();
			thread.join();
			times[i] = (System.nanoTime() - start) / 1.0e9;
		}
		
		double mean = 0;
		for (int i = 0; i < trials; i++) {
			mean += times[i];
		}
		mean /= trials;
		
		double stddev = 0;
		for (int i = 0; i < trials; i++) {
			stddev += Math.pow(times[i] - mean, 2);
		}
		stddev /= trials - 1;
		
		return new double[] {mean, stddev};
	}
	
	public static void main(String[] args) throws Exception {
		System.out.println("Starting tests\n");
		
		File auto = new File("data/alice.txt");
		double[] data;
		
		System.out.printf("Varying k, using random text length %d and file length %d (alice.txt)\n", 100, auto.length());
		for (int i = 1; i <= 15; i++) {
			data = benchmark(100, i, new Scanner(auto));
			System.out.printf("k: %d \t mean: %f \t stddev %f \t ci: [%f, %f]\n", i, data[0], data[1], data[0] - 1.96 * data[1], data[0] + 1.96 * data[1]);
		}
		
		System.out.println();
		
		System.out.printf("Varying text length, using k %d and file length %d (alice.txt)\n", 5, auto.length());
		for (int i = 20; i <= 300; i += 20) {
			data = benchmark(i, 5, new Scanner(auto));
			System.out.printf("text length: %d \t mean: %f \t stddev: %f \t ci: [%f, %f]\n", i, data[0], data[1], data[0] - 1.96 * data[1], data[0] + 1.96 * data[1]);
		}
		
		System.out.println();
		
		File[] files = new File[] {new File("data/alice.txt"), new File("data/clinton-08"),
								   new File("data/clinton-nh.txt"), new File("data/edwards-nh.txt"),
								   new File("data/hawthorne.txt"), new File("data/kjv10.txt"),
								   new File("data/melville.txt"), new File("data/obama-nh.txt"),
								   new File("data/poe.txt"), new File("data/romeo.txt")};
		Arrays.sort(files, (a, b) -> {
			return (int) (a.length() - b.length());
		});
		
		System.out.printf("Varying file length, using k %d and text length %d\n", 5, 100);
		for (File f : files) {
			int uniqueKeys = getGenerator().train(new Scanner(f), "", 5);
			data = benchmark(100, 5, new Scanner(f));
			System.out.printf("unique keys: %d \t mean: %f \t stddev %f \t ci: [%f, %f]\n", uniqueKeys, data[0], data[1], data[0] - 1.96 * data[1], data[0] + 1.96 * data[1]);
		}
		
		System.out.println();
		System.out.println("Finished tests");
	}
}