/**
 *	Main for Markov Text Generation Program
 * 
 *	@author Mike Ma
 */

import javafx.application.Application;
import javafx.stage.Stage;

public class MarkovMain extends Application {
	
	/*
	 *	Change the value of randomSeed to check other possibilities.
	 *	You can also create the TextGenerator without the seed to
	 *	get non-deterministic behavior.
	 *
	 *	Likewise, simply change BruteGenerator to MapGenerator to use
	 *	the alternative implementation.
	 *
	 *	NOTE: you should have an ERROR in this class when you download it
	 *	since you haven't implemented your TextGenerators yet.
	 */
	public void start(Stage stage) throws Exception {
		int randomSeed =  54321;
		TextGenerator generator = new BruteGenerator(randomSeed);
		new MarkovGUI(stage, generator);
	}
	
	public static void main(String[] args) {
		launch(args);
	}
}