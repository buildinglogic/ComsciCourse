import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

import javax.swing.JFileChooser;

import princeton.StdAudio;
import princeton.StdDraw;

public class NBody{
	
    public static final double G = 6.67E-11;

    /**
     * returns Euclidean distance between (x1, y1) and (x2, y2)
     *
     * @param x1
     *            x-coordinate of point 1
     * @param y1
     *            y-coordinate of point 1
     * @param x2
     *            x-coordinate of point 2
     * @param y2
     *            y-coordinate of point 2
     * @return Euclidean distance between (x1, y1) and (x2, y2)
     */
    public double distance(double x1, double y1, double x2, double y2) {
    	double Distance;
    	Distance=Math.sqrt(Math.pow((x1-x2), 2)+Math.pow((y1-y2), 2));
    	//TODO: Complete distance
        return Distance;
    }
    
    
    /**
     * return the magnitude of the gravitational force between two bodies of
     * mass m1 and m2 that are distance r apart
     *
     * @param m1
     *            mass of body 1 in kg
     * @param m2
     *            mass of body 2 in kg
     * @param r
     *            distance in m
     * @return force between m1 and m2 that are distance r apart
     */
    public double force(double m1, double m2, double r) {
        double Force;
        Force=G*m1*m2/(r*r);
    	//TODO: Complete force
        return Force;
    }


    /**
     * Returns the x positions and y positions of bodies
     * @param totalTime The total amount of universe time to run for
     * @param timeStep The value of delta t in the equations to calculate position
     * @param info The scanner with info about the initial conditions of the
     * bodies
     * @return an array whose first element is the x and y position of the first body, and so on
     * t = totalTime
     */
    public double[][] positions(Scanner info, int totalTime, int timeStep) {
    	int nplanet=info.nextInt();//nplanet should equal to 5 if 5 planets exist
        double universeradius=info.nextDouble();
        double[] x=new double[nplanet];
        double[] y=new double[nplanet];
        double[] vx=new double[nplanet];
        double[] vy=new double[nplanet];
        double[] mass=new double[nplanet];
        String[] image=new String[nplanet];
        for(int i=0; i<nplanet; i++){
        	x[i]=info.nextDouble();
        	y[i]=info.nextDouble();
        	vx[i]=info.nextDouble();
        	vy[i]=info.nextDouble();
        	mass[i]=info.nextDouble();
        	image[i]=info.next();
        }
        StdDraw.picture(0.0, 0.0, "data/starfield.jpg");
    	StdDraw.setXscale(-universeradius, universeradius);
    	StdDraw.setYscale(-universeradius, universeradius);
    	for(int i=0; i<nplanet; i++){
    		StdDraw.picture(x[i], y[i], "data/"+image[i]);
    	}
        for(int i=0; i<totalTime; i+=timeStep){//find position after each "timeStep"
        	double[][] Force=new double[nplanet][2];
    		double[][] acceleration=new double[nplanet][2];//two-D array of acceleration {(ax1,ay1),(ax2,ay2),...}
    		for(int l1=0; l1<nplanet; l1++){//clear the force and acceleration array before each calculation
    			for(int l2=0; l2<2; l2++){
    				Force[l1][l2]=0.0;
    				Force[l1][l2]=0.0;
    				acceleration[l1][l2]=0.0;
    				acceleration[l1][l2]=0.0;
    			}
    		}
        	for(int j=0; j<nplanet; j++){//find each body position at each "timeStep"
        			for (int n=0; n<nplanet; n++){//calculate the x-force or y-force on k-th body 
        				if (n!=j){ 
        				 double r=distance(x[j],y[j],x[n],y[n]);
        				 double F=force(mass[j],mass[n],r); 
        				 Force[j][0]+=F*(x[n]-x[j])/r;//Fx=F*cos(theda); x[n]-x[k] can't be x[k]-x[n], the + or - means direction
        				 Force[j][1]+=F*(y[n]-y[j])/r;
        				 }
        		    }
        	    //calculate the x-acceleration or y-acceleration
        	    acceleration[j][0]=Force[j][0]/mass[j];
        	    acceleration[j][1]=Force[j][1]/mass[j];  
        	    vx[j]+=timeStep*acceleration[j][0];//new velocity
        	    vy[j]+=timeStep*acceleration[j][1];
        	}//finish update the new positions and velocity
        	for(int j=0; j<nplanet; j++){
        		x[j]+=timeStep*vx[j];
        	    y[j]+=timeStep*vy[j];
        	}
        	StdDraw.clear();
        	StdDraw.picture(0.0, 0.0, "data/starfield.jpg");
        	StdDraw.setXscale(-universeradius, universeradius);
        	StdDraw.setYscale(-universeradius, universeradius);
        	for(int i1=0; i1<nplanet; i1++){
        		StdDraw.picture(x[i1], y[i1], "data/"+image[i1]);
        	}
        	StdDraw.show(30);
        }
        double[][] output = new double[nplanet][2]; //put new position into a 2-D array
        for(int i=0; i<nplanet; i++){
        	output[i][0]=x[i];
        	output[i][1]=y[i];
        }

        return output;
    }

    public static void main(String[] args) throws FileNotFoundException {
        Scanner info = openFile();
        int time = 10000000;
        int dt = 25000;
       
        if (info != null) {
            //StdAudio.play("data/2001.mid");
            NBody myNBody = new NBody();
            double[][] results = myNBody.positions(info, time, dt);
            for(int i = 0; i < results.length; i++) {
                for(int j = 0; j < results[i].length; j++) {
                    System.out.print(results[i][j]+" ");
                }
                System.out.println();
            }
//            double[][] test = myNBody.positions(new Scanner(new File("data/planets.txt")), 100000, 25000);
//            for(int i = 0; i < test.length; i++) {
//                for(int j = 0; j < test[i].length; j++) {
//                    System.out.print(test[i][j]+" ");
//                }
//                System.out.println();
//            }
            StdAudio.close();
        }
    }
    /**
     * Displays file chooser for browsing in the project directory. and opens
     * the selected file
     *
     * @return a new Scanner that produces values scanned from the selected
     *         file. null if file could not be opened or was not selected
     */
    
    public static Scanner openFile() {
        Scanner scan = null;
        System.out.println("Opening file dialog.");
        JFileChooser openChooser = new JFileChooser(System.getProperties()
                                                    .getProperty("user.dir"));
        
        int retval = openChooser.showOpenDialog(null);
        if (retval == JFileChooser.APPROVE_OPTION) {
            File file = openChooser.getSelectedFile();
            System.out.println(file.getAbsolutePath());
            try {
                scan = new Scanner(file);
                System.out.println("Opening: " + file.getName() + ".");
            } catch (FileNotFoundException e) {
                System.out.println("Could not open selected file.");
                e.printStackTrace();
            }
        } else {
            System.out.println("File open canceled.");
            System.exit(0);
        }
        
        return scan;
    }
}