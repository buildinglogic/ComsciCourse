import java.util.Arrays;
import java.util.List;

public class NGram implements Comparable<NGram> {

	private String[] contents;
	private String separator;
	
	public NGram(List<String> source, String sep) {
		separator = sep;
		contents = new String[source.size()];
		contents = Arrays.copyOf(source.toArray(new String[source.size()]), source.size());
	}

	public int compareTo(NGram other) {
		//"this" compared to "other"; both are string array
		for(int i=0; i<this.contents.length && i<other.contents.length; i++){
			if(this.contents[i].compareTo(other.contents[i])<0) {return -1;}
			if(this.contents[i].compareTo(other.contents[i])>0) {return 1;}
			if(this.contents[i].compareTo(other.contents[i])==0) {
				if (i==this.contents.length-1 && i<other.contents.length-1) {return -1;}//"ab""bc"; "ab" "bc" "cd"
				if (i<this.contents.length-1 && i==other.contents.length-1) {return 1;}//"ab""bc""cd"; "ab" "bc"
			}	
		}
		return 0;
	}

	public boolean equals(Object o) {
		//cast object o to NGram, then compare equals or not
		if(((NGram) o) instanceof NGram) {
			if (this.contents.length!=((NGram) o).contents.length) { return false;}
			else {
				for (int i=0; i<this.contents.length; i++) {
					if (!((NGram) o).contents[i].equals(this.contents[i])) {return false;}
				}
			}
			return true;//this equals to o
		}
		return false;//object o can't be casted into NGram
	}

	public int hashCode() {
		//hashCode for NGram, whose contents are a string array
		//the following code was based on http://stackoverflow.com/questions/2730865/how-do-i-calculate-a-good-hash-code-for-a-list-of-strings
		final int prim=31;
		int code=1;
		for(int i=0; i<this.contents.length;i++){
			code=code*prim+this.contents[i].hashCode();
		}
		return code;
//		return 33333;
	}

	public String toString() {
		return this.contents[(this.contents.length-1)].toString()+separator;
	}
}