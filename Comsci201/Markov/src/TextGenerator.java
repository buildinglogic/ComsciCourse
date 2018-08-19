/**
 *	Interface for Markov Chain Random Text Generation.  Specifies
 *	the public API for training the Markov model from a scanner and
 *	generating a fixed amount of random text.
 *	
 *	@author Brian Lavallee
 *	@contributor Vinay Kshirsagar
 *	@contributor Owen Astrachan
 */
import java.util.Scanner;

public interface TextGenerator {
	
	/**
	 *	Takes in a suitably long source of text in the desired
	 *	language to be used as the Markov model for possible states
	 *	and state changes.  Typically, these arguments will be
	 *	passed directly to the TrainingText object, a useful
	 *	abstraction.  However, there may be other cases where
	 *	additional work my be beneficial.  Train is called whenever
	 *	the source, delimiter, or k changes.
	 *
	 *	@param source
	 *		Scanner which can iterate over the training text.
	 *
	 *	@param delimiter
	 *		String used by the Scanner to distinguish between a
	 *		word Markov model and a letter model.
	 *
	 *	@param k
	 *		The length of the k-gram to use.  Defines the size of
	 *		the previous state to use when calculating the next state.
	 *
	 *	@return
	 *		The number of states in the model.
	 */
	public int train(Scanner source, String delimiter, int k);
	
	/**
	 *	Generates random text of length length using a Markov model.
	 *	Note that length does not specify a number of words or a number
	 *	of letters, specifically, but works both ways depending on
	 *	the mode in which TextGenerator is.
	 *
	 *	@param length
	 * 		Either the number of words or the number of letters of
	 * 		random text to generate.
	 * 
	 *	@return
	 *		Pseudo-random text based on the Markov model.
	 */
	public String generateText(int length);
}