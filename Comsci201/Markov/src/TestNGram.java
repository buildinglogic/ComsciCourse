import org.junit.*;
import java.util.*;

import static org.junit.Assert.*;

public class TestNGram {
	;
	private NGram[] myNgrams;

	@Before
	public void setUp() {
		String str = "aa bb cc aa bb cc aa bb cc aa bb dd ee ff gg hh ii jj";
		String[] array = str.split("\\s+");
		List<String> elements = Arrays.asList(array);
		myNgrams = new NGram[array.length - 2];
		for (int k = 0; k < array.length - 2; k++) {
			myNgrams[k] = new NGram(elements.subList(k, k + 3), " ");
		}
	}

	@Test
	public void testHashEquals() {
		assertEquals("hash fail on equals 0,3", myNgrams[0].hashCode(), myNgrams[3].hashCode());
		assertEquals("hash fail on equals 0,3", myNgrams[0].hashCode(), myNgrams[6].hashCode());
		assertEquals("hash fail on equals 0,3", myNgrams[1].hashCode(), myNgrams[4].hashCode());
		assertEquals("hash fail on equals 0,3", myNgrams[2].hashCode(), myNgrams[8].hashCode());
		assertEquals("hash fail on equals 0,3", myNgrams[2].hashCode(), myNgrams[5].hashCode());
	}

	@Test
	public void testEquals() {
		assertEquals("fail on 0,3", myNgrams[0].equals(myNgrams[3]), true);
		assertEquals("fail on 0,6", myNgrams[0].equals(myNgrams[6]), true);
		assertEquals("fail on 1,4", myNgrams[1].equals(myNgrams[4]), true);
		assertEquals("fail on 2,5", myNgrams[2].equals(myNgrams[5]), true);
		assertEquals("fail on 2,8", myNgrams[2].equals(myNgrams[8]), true);
		assertEquals("fail on 0,2", myNgrams[0].equals(myNgrams[2]), false);
		assertEquals("fail on 0,4", myNgrams[0].equals(myNgrams[2]), false);
		assertEquals("fail on 2,3", myNgrams[2].equals(myNgrams[3]), false);
		assertEquals("fail on 2,6", myNgrams[2].equals(myNgrams[6]), false);
		assertEquals("fail on 7,8", myNgrams[7].equals(myNgrams[8]), false);
		assertEquals("fail on non-NGram input", myNgrams[0].equals("abc"), false);
	}

	@Test
	public void testCompareTo() {
		assertEquals("fail on 0,3", myNgrams[0].compareTo(myNgrams[3]) == 0, true);
		assertEquals("fail on 0,6", myNgrams[0].compareTo(myNgrams[6]) == 0, true);
		assertEquals("fail on 0,4", myNgrams[0].compareTo(myNgrams[4]) < 0, true);
		assertEquals("fail on 7,8", myNgrams[7].compareTo(myNgrams[8]) < 0, true);
		assertEquals("fail on 12,11", myNgrams[12].compareTo(myNgrams[11]) > 0, true);
		List<String> source = Arrays.asList("aa", "bb", "cc", "dd");
		NGram fourgram = new NGram(source, " ");
		assertEquals("fail on different-sized inputs", myNgrams[0].compareTo(fourgram) < 0, true);
		source = Arrays.asList("a", "b", "c", "d");
		fourgram = new NGram(source, "");
		assertEquals("fail on different-sized inputs", myNgrams[0].compareTo(fourgram) > 0, true);
	}

	@Test
	public void testHash() {
		Set<Integer> set = new HashSet<Integer>();
		for (NGram w : myNgrams) {
			set.add(w.hashCode());
		}

		assertTrue("hash code test", set.size() > 9);
	}
}
