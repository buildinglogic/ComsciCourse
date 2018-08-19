
import java.util.ArrayList;
import java.util.Random;
import java.util.Scanner;


public class BruteGenerator implements TextGenerator {
	
	Random rnd;
	TrainingText new_TT;
	
	public BruteGenerator(){
		rnd=new Random();
	}
	
	public BruteGenerator(int n){
		rnd=new Random(n);
	}

	public int train(Scanner source, String delimiter, int k) {
		new_TT = new TrainingText(source, delimiter, k);
		return new_TT.size();
	}
	
	public String generateText(int length){
		StringBuffer output=new StringBuffer();
		NGram seed = new_TT.get(rnd.nextInt(new_TT.size()));
		for(int i=0; i<length; i++){//generate  new text with length "length"
			ArrayList<NGram> followstate = new ArrayList<NGram>();//the compiled list of the following states after "seed"
			for (int j=0; j<new_TT.size();j++){//last NGram has not "followstate"
				int index=new_TT.indexOf(seed, j);
				if(index==(new_TT.size()-1)||index==new_TT.size()){break;}
				//index=new_TT.size()-1, then, the following NGram is null; if index=new_TT.size(), no found seed, then break.
				followstate.add(new_TT.get(index+1));//add the next NGram after "seed" to the compiled list
				j=index;
			}//end of creating the compiled following states for the "seed"
			output.append(seed.toString());
			seed = followstate.get(rnd.nextInt(followstate.size()));//update seed with one random new following states
		}
		return output.toString();
	}

}
