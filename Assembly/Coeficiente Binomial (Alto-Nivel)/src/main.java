
public class main {
	 public static void main(String[] args) {
		 	System.out.println(binomial(5,2));		 	
	    }
	 
	 
	 public static double fatorial(double valor) {
		 double resultado = 1 ;
		 while(valor != 0) {
			 resultado = resultado * valor;
			 valor -= 1;
		 }
		 return resultado;
	 }
	 
	 /**
	 	Tomando em conta a binomial na forma
	 	(n)
	 	(k)
	 	= n!/(k!*(n-k)!)
	 */
	 public static double binomial(int n, int k) {
		 return fatorial(n)/(fatorial(k)*fatorial(n-k));
	 }
}
