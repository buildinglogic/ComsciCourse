import java.util.Comparator;
import java.util.PriorityQueue;

/**
 * General trie/priority queue algorithm for implementing Autocompletor
 * 
 * @author Austin Lu
 *
 */
public class TrieAutocomplete implements Autocompletor {

	/**
	 * Root of entire trie
	 */
	protected Node myRoot;

	/**
	 * Constructor method for TrieAutocomplete. Should initialize the trie
	 * rooted at myRoot, as well as add all nodes necessary to represent the
	 * words in terms.
	 * 
	 * @param terms
	 *            - The words we will autocomplete from
	 * @param weights
	 *            - Their weights, such that terms[i] has weight weights[i].
	 * @throws a
	 *             NullPointerException if either argument is null
	 */
	public TrieAutocomplete(String[] terms, double[] weights) {
		if (terms == null || weights == null)
			throw new NullPointerException("One or more arguments null");
		// Represent the root as a dummy/placeholder node
		myRoot = new Node('-', null, 0);

		for (int i = 0; i < terms.length; i++) {
			add(terms[i], weights[i]);
		}
	}

	/**
	 * Add the word with given weight to the trie. If word already exists in the
	 * trie, no new nodes should be created, but the weight of word should be
	 * updated.
	 * 
	 * In adding a word, this method should do the following: Create any
	 * necessary intermediate nodes if they do not exist. Update the
	 * subtreeMaxWeight of all nodes in the path from root to the node
	 * representing word. Set the value of myWord, myWeight, isWord, and
	 * mySubtreeMaxWeight of the node corresponding to the added word to the
	 * correct values
	 * 
	 * @throws a
	 *             NullPointerException if word is null
	 * @throws an
	 *             IllegalArgumentException if weight is negative.
	 * 
	 */
	private void add(String word, double weight) {
		
		if (word == null) {
			throw new NullPointerException();
		}
		if (weight < 0) {
			throw new IllegalArgumentException();
		}
		
		Node current = myRoot;
		boolean decrease = false;
		for(int i = 0; i < word.length(); i++){
			char nextCh = word.charAt(i);
			if (current.mySubtreeMaxWeight < weight) {
				current.mySubtreeMaxWeight = weight;
			}
			if (!current.children.containsKey(nextCh)) {
				current.children.put(nextCh, new Node(nextCh, current, weight));
			}
			current = current.children.get(nextCh);
			}
		current.isWord = true;
		if (current.myWeight>weight) {//decrease the weight
			decrease = true;
		}
		current.myWeight = weight;//maybe decrease the weight
		current.myWord = word;
		if (current.mySubtreeMaxWeight<weight) {
			current.mySubtreeMaxWeight = weight;
		}
		//update the "subtreeMaximumWeight"of all nodes in the path from root to the node representing word
		if (decrease) {
			for(int i = 0; i < word.length(); i++){
			    double maxWeight = current.myWeight;
			    for (Character t : current.children.keySet()) {
			    	if (current.children.get(t).mySubtreeMaxWeight > maxWeight) {
			    		maxWeight = current.children.get(t).mySubtreeMaxWeight;
			    	}
			    }
			    current.mySubtreeMaxWeight = maxWeight;
				current = current.parent;
				}
		}
	}

	@Override
	/**
	 * Required by the Autocompletor interface. Returns an array containing the
	 * k words in the trie with the largest weight which match the given prefix,
	 * in descending weight order. If less than k words exist matching the given
	 * prefix (including if no words exist), then the array instead contains all
	 * those words. e.g. If terms is {air:3, bat:2, bell:4, boy:1}, then
	 * topKMatches("b", 2) should return {"bell", "bat"}, but topKMatches("a",
	 * 2) should return {"air"}
	 * 
	 * @param prefix
	 *            - A prefix which all returned words must start with
	 * @param k
	 *            - The (maximum) number of words to be returned
	 * @return An array of the k words with the largest weights among all words
	 *         starting with prefix, in descending weight order. If less than k
	 *         such words exist, return an array containing all those words If
	 *         no such words exist, return an empty array
	 * @throws a
	 *             NullPointerException if prefix is null
	 */
	public String[] topKMatches(String prefix, int k) {

		if (prefix == null) {
			throw new NullPointerException();
		}
		//queue pq, descending SubtreeMaxWeight order
		PriorityQueue<Node> pq = new PriorityQueue<Node>(k, new Node.ReverseSubtreeMaxWeightComparator());
		//queue L, ascending myWeight order
		PriorityQueue<Node> L = new PriorityQueue<Node>(k, new TrieAutocomplete.myWeightComparator());

		Node current = myRoot;
		for(int i = 0; i < prefix.length(); i++){
			char nextCh = prefix.charAt(i);
			if (!current.children.containsKey(nextCh)) {
				String[] empty_ret = new String[0];
				return empty_ret;
			}
			current = current.children.get(nextCh);
		}
		int count = 0;
		pq.add(current);
		while (pq.size()!=0 && count<k) {
			current = pq.peek();
			if (current.isWord) {
				if (current.myWeight == current.mySubtreeMaxWeight) {
					count ++;
				}
				if (L.size()<k) {
					L.add(current);
				} else if (current.myWeight > L.peek().getWeight()) {
					L.remove();
					L.add(current);
				}
			}
			Node temp = pq.remove();
			for (Character t : temp.children.keySet()) {
				pq.add(temp.children.get(t));
			}
		}

		//take the first k Nodes from L queue (descending myWeight order already)
		 int numResults = Math.min(k, L.size());
		    String[] ret = new String[numResults];
		    for (int i = 0; i < numResults; i++) {
		      ret[numResults - 1 - i] = L.remove().getWord();
		    }
		    return ret;
	}
	
	//add one comparator, ascending myWeight order
	public static class myWeightComparator implements Comparator<Node> {
		
		public int compare(Node o1, Node o2) {
			if (o1.myWeight < o2.myWeight) {
				return -1;
			} else if (o1.myWeight > o2.myWeight) {
				return 1;
			}
			return 0;
		}
	}

	@Override
	/**
	 * Given a prefix, returns the largest-weight word in the trie starting with
	 * that prefix.
	 * 
	 * @param prefix
	 *            - the prefix the returned word should start with
	 * @return The word from _terms with the largest weight starting with
	 *         prefix, or an empty string if none exists
	 * @throws a
	 *             NullPointerException if the prefix is null
	 * 
	 */
	public String topMatch(String prefix) {
		
		if (prefix == null) {
			throw new NullPointerException();
		}
		
		Node current = myRoot;
		for(int i = 0; i < prefix.length(); i++){
			char nextCh = prefix.charAt(i);
			if (!current.children.containsKey(nextCh)) {
				return "";
			}
			current = current.children.get(nextCh);
		}
		double maxsubtree = current.mySubtreeMaxWeight;
		while (maxsubtree != current.myWeight) {
			for (Character t : current.children.keySet()) {
				if (current.children.get(t).mySubtreeMaxWeight == maxsubtree) {
					char nextCh = t;
					current = current.children.get(nextCh);
					break;
				}
				
			}
		}
		return current.myWord;
	}

}
