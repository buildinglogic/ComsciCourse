import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class TrainingText {

	private List<String> textModel;
	private int k;
	private String separator;

	public TrainingText(Scanner source, String delimiter, int k) {
		separator = delimiter.equals("\\s+") ? " " : "";
		this.k = k;
		source.useDelimiter(delimiter);
		textModel = new ArrayList<>();
		while (source.hasNext()) {
			String next = source.next();
			next = next.matches("\\s+") ? " " : next;
			textModel.add(next);
		}
		//trim the end-of-file; ??why startpos is "1" instead of "0";don't how this trim work and why
		for (int i = size() - 1; i >= 0; i--) {
			NGram ngram = get(i);
			if (indexOf(ngram, 1) >= i) {
				textModel.remove(i + k - 1);
			}
			else {
				break;
			}
		}
	}

	public int indexOf(NGram seed, int startPos) {
		for (int i = startPos; i < size(); i++) {
			if (get(i).equals(seed)) {
				return i;
			}
		}
		return size();
	}

	public int size() {
		return textModel.size() - k + 1;
	}

	public NGram get(int index) {
		return new NGram(textModel.subList(index, index + k), separator);
	}
}