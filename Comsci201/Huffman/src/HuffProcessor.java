import java.util.PriorityQueue;

public class HuffProcessor implements Processor{
	
	
	public void compress(BitInputStream in, BitOutputStream out){
		
		//count the character frequency from "in"
		int[] inputFrequency = new int[256];
		int readtemp = in.read();
		while (readtemp != -1) {
			inputFrequency[readtemp]++;
			readtemp = in.read();
		}
		in.reset();
		
		//create Huffman tree
		PriorityQueue<HuffNode> pq = new PriorityQueue<HuffNode>();
		for(int i = 0; i<=255; i++){
			if (inputFrequency[i] != 0) {
				pq.add(new HuffNode(i,inputFrequency[i]));
			}
		}
		pq.add(new HuffNode(PSEUDO_EOF, 0));
		
		//print out the alphabet size
//		System.out.println(pq.size());
		
		while(pq.size()>1){
			HuffNode leftnode = pq.poll();
			HuffNode rightnode = pq.poll();
			pq.add(new HuffNode(-1, leftnode.weight()+rightnode.weight(),leftnode, rightnode ));
		}
		
		//traverse tree and extract codes
		String[] HuffcodeArray = new String[257];//the last one is the PSEUDO-EOF
		String path = "";
		HuffNode current = pq.poll();
		extraCodes(current, path, HuffcodeArray);
		
		//write the header
		out.writeBits(32, HUFF_NUMBER);
		writeHeader(current, out);
	    
	    //compress or write the body
		int readtemp2 = in.read();
		while (readtemp2 != -1){
			String code = HuffcodeArray[readtemp2];
			out.writeBits(code.length(), Integer.parseInt(code, 2));
			readtemp2 = in.read();
		}
		
		//write the pseudo-EOF
		out.writeBits(HuffcodeArray[256].length(), Integer.parseInt(HuffcodeArray[256],2));
		
	}
	
	//helper method extraCodes for "compress"
	private void extraCodes(HuffNode current, String path, String[] HuffcodeArray){
		if (current.left() == null && current.right() == null){
			HuffcodeArray[current.value()] = path;
			return;
		}
		extraCodes(current.left(), path+0, HuffcodeArray);
		extraCodes(current.right(), path+1, HuffcodeArray);
	}
	
	//helper method writeHeader for "compress"
	private void writeHeader(HuffNode current, BitOutputStream out){
		if (current.left() == null && current.right() == null){
			out.writeBits(1, 1);
			out.writeBits(9, current.value());
			return;
		}
		out.writeBits(1,0);
		writeHeader(current.left(), out);
		writeHeader(current.right(), out);
	}
	
	public void decompress(BitInputStream in, BitOutputStream out){
		
		//check whether decompressable
		if (in.readBits(32) != HUFF_NUMBER) {
			throw new HuffException("error");
		}
		
		//recreate HuffMan tree from header
		HuffNode HufftreeHead = readHeader(in);
		
		//parse body of compressed file
		HuffNode current = HufftreeHead;
		int readtemp3 = in.readBits(1);
		while (readtemp3 != -1) {
			if (readtemp3 == 1){
				current = current.right();
			}
			else {
				current = current.left();
			}
			if (current.left() == null && current.right() == null){
				if (current.value() == PSEUDO_EOF) {
					return;
				}
				else {
					out.writeBits(8, current.value());
					current = HufftreeHead;
				}
			}
			readtemp3 = in.readBits(1);
		}
		
	}
	
	//helper method readHeader for "decompress"
	private HuffNode readHeader(BitInputStream in){
		if (in.readBits(1) == 0){
			HuffNode left = readHeader(in);
			HuffNode right = readHeader(in);
			return new HuffNode(-1, left.weight()+right.weight(),left, right );
		}
		else {
			return new HuffNode(in.readBits(9), 0);
		}
	}
	
}	
	

