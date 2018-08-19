import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;
import java.util.Scanner;

public class MapGenerator implements TextGenerator{
	
	private TrainingText new_TT;
	private HashMap<NGram, ArrayList<NGram>> stateMap = new HashMap<NGram, ArrayList<NGram>>();;
	private Random rnd;
	
	
	public MapGenerator(){
		rnd=new Random();
	}
	
	public MapGenerator(int n){
		rnd=new Random(n);
	}
	
	public int train(Scanner source, String delimiter, int k){
		new_TT = new TrainingText(source, delimiter, k);
		for(int i=0; i<new_TT.size()-1; i++){//the last NGram has not "followstate", so i<new_TT.size()-1
			if(!stateMap.containsKey(new_TT.get(i))){//if this state is not in the Map yet
				ArrayList<NGram> followstates=new ArrayList<NGram>();
				followstates.add(new_TT.get(i+1));
				stateMap.put(new_TT.get(i), followstates);
			}
			else {//if this state is in the Map, then update the following sates list
				stateMap.get(new_TT.get(i)).add(new_TT.get(i+1));
			}
		}
		return stateMap.keySet().size();//return the size of the map
	}
	
	public String generateText(int length){
		StringBuffer output= new StringBuffer();
		NGram seed = new_TT.get(rnd.nextInt(new_TT.size()));
		int random1 = 0;
		for(int i=0; i<length; i++){//generate  new text with length "length"
			random1 = rnd.nextInt(stateMap.get(seed).size());//random number under the size of list of the value in map
			output.append(seed.toString());
			seed = stateMap.get(seed).get(random1);
		}
		return output.toString();
	}

}
