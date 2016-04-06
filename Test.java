import joos.lib.*;

/*
public class Test {
    public Test() {
        super();
    }

    public void ff(int i, int j) {
        JoosIO f;
        f = new JoosIO();
        f.println(i+"");
        f.println(j+"");
    }

    public static void main(String argv[]) {
        int k1, k2, k3, k4 ;

        k1 = k2 = k3 = k4 = 0 ;
        new Test().ff(k4, k4);
    }
    
}
*/

public class Test {
	protected boolean b;

	public Test() {
        super();
		b = false;
    }

  public void g() {
	JoosIO f;
	f = new JoosIO();
   if ( b == true )
	f.println("true");
   else 
	f.println("false");
 }
	
   public static void main (String args[]) { 
        new Test().g();
   }
}
