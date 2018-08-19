import static org.junit.Assert.*;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;

import org.junit.Before;
import org.junit.Test;

public class TestGenerate {
	
	private String[][] letterAnswer;
	private String[][] wordAnswer;
	
	@Before
	public void setup() {
		letterAnswer = new String[2][2];
		letterAnswer[0][0] = "tly the whole         By thistle, the  rose up like the took them with a showing nothing itself";
		letterAnswer[0][1] = "went to be said, turned round its nose, by with a hurriedly break the court, she's so ofte";
		letterAnswer[1][0] = " upon me, or apprentially unhallowed with high excitement that if his face was nothing";
		letterAnswer[1][1] = "his procedure with Bartleby.''  ``Who are always there I had receive himsel";
		
		wordAnswer = new String[2][2];
		wordAnswer[0][0] = "to take MORE than nothing.' `Nobody asked YOUR opinion,' said Alice. `Who's making ";
		wordAnswer[0][1] = "should think you could draw treacle out of a treacle-well--eh, stupid?' `But they were IN ";
		wordAnswer[1][0] = "then, if no better motive can be enlisted, should, especially with high-tempered men";
		wordAnswer[1][1] = "Yet a certain melancholy mixed with this: I was almost sorry for my ";
	}
	
	@Test
	public void testBrute() {
		try {
			BruteGenerator tg = new BruteGenerator(54321);
			
			BufferedInputStream alice = new BufferedInputStream(new FileInputStream("data/alice.txt"));
			alice.mark(Integer.MAX_VALUE);
			
			tg.train(new Scanner(alice), "", 5);
			String result = tg.generateText(1000);
			assertTrue(result.startsWith(letterAnswer[0][0]));
			assertTrue(result.endsWith(letterAnswer[0][1]));
			
			alice.reset();
			
			tg.train(new Scanner(alice), "\\s+", 5);
			result = tg.generateText(1000);
			assertTrue(result.startsWith(wordAnswer[0][0]));
			assertTrue(result.endsWith(wordAnswer[0][1]));
			
			BufferedInputStream melville = new BufferedInputStream(new FileInputStream("data/melville.txt"));
			melville.mark(Integer.MAX_VALUE);
			
			tg.train(new Scanner(melville), "", 5);
			result = tg.generateText(1000);
			assertTrue(result.startsWith(letterAnswer[1][0]));
			assertTrue(result.endsWith(letterAnswer[1][1]));
			
			melville.reset();
			
			tg.train(new Scanner(melville), "\\s+", 5);
			result = tg.generateText(1000);
			assertTrue(result.startsWith(wordAnswer[1][0]));
			assertTrue(result.endsWith(wordAnswer[1][1]));
		}
		catch (FileNotFoundException fnf) {
			fnf.printStackTrace();
		}
		catch (IOException io) {
			io.printStackTrace();
		}
	}
	

	@Test
	public void testMap() {
		try {
			MapGenerator tg = new MapGenerator(54321);
			
			BufferedInputStream alice = new BufferedInputStream(new FileInputStream("data/alice.txt"));
			alice.mark(Integer.MAX_VALUE);
			
			tg.train(new Scanner(alice), "", 5);
			String result = tg.generateText(1000);
			assertTrue(result.startsWith(letterAnswer[0][0]));
			assertTrue(result.endsWith(letterAnswer[0][1]));
			
			alice.reset();
			
			tg.train(new Scanner(alice), "\\s+", 5);
			result = tg.generateText(1000);
			assertTrue(result.startsWith(wordAnswer[0][0]));
			assertTrue(result.endsWith(wordAnswer[0][1]));
			
			BufferedInputStream melville = new BufferedInputStream(new FileInputStream("data/melville.txt"));
			melville.mark(Integer.MAX_VALUE);
			
			tg.train(new Scanner(melville), "", 5);
			result = tg.generateText(1000);
			assertTrue(result.startsWith(letterAnswer[1][0]));
			assertTrue(result.endsWith(letterAnswer[1][1]));
			
			melville.reset();
			
			tg.train(new Scanner(melville), "\\s+", 5);
			result = tg.generateText(1000);
			assertTrue(result.startsWith(wordAnswer[1][0]));
			assertTrue(result.endsWith(wordAnswer[1][1]));
		}
		catch (FileNotFoundException fnf) {
			fnf.printStackTrace();
		}
		catch (IOException io) {
			io.printStackTrace();
		}
	}
}