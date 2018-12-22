import java.util.*;
import java.lang.*;


public class HighScoreList extends Object {
HSob scores[];
int NUM_SCORES=10;
public long lastChange = 0;

HighScoreList(int num,String str) {
	NUM_SCORES=num;
	scores = new HSob[NUM_SCORES];
	parseData(str);
}

HighScoreList(int num) {
	NUM_SCORES = num;
	scores = new HSob[NUM_SCORES];
}

public void parseData(String str) {
StringTokenizer st1=new StringTokenizer(str,"||");
String theStr;
		while (st1.hasMoreTokens()  ) {
		theStr = st1.nextToken();		
		if (! theStr.startsWith("null")) 
			addScore( new HSob (theStr));
			}
}

public String tryScore( String data) {

HSob temp = new HSob (data);
if (addScore(temp) > 0)
	return temp.toDataString();
else
	return null;
}

public String tryScore( String name, float score, String email, String other) {
HSob temp;

temp = new HSob (name, score, email, other);
return tryScore (temp.toDataString());
}


public int addScore(HSob sc) {
int x,i;
x=0;

if (sc==null) return 0;

lastChange = System.currentTimeMillis();

while (x<NUM_SCORES) {

if (scores[x] == null || sc.score > scores[x].score) {
	for( i=NUM_SCORES-2 ; i>=x ; i--) 
		scores[i+1]=scores[i];
	scores[x]= sc;

	return x+1;
	}
x++;
}

return 0;
}	

public HSob getScore(int num) {
if (num < NUM_SCORES)
	return scores[num];
else
	return null;
}

public String toDataString() {
String temp= "";
int x;

for(x=0;x<NUM_SCORES;x++)
	if(scores[x]==null) temp+="null||";
	else
	temp+=scores[x].toDataString()+"||";
return temp;
}

}
class HSob extends Object {

	public String name=null,email,other;
	public float score = -1;

	HSob(String n, float sc, String em, String ot) {
		name=n;
		score=sc;
		email = em;
		other = ot;
	}

	HSob(String data) {
		Float fl = null;
		String str;
		StringTokenizer st = new StringTokenizer(data, "&");

	while (st.hasMoreTokens()) {
		 str = st.nextToken();
		if (name == null)
			name = str;
		else
		try {		
			fl = Float.valueOf( str );
			score = fl.floatValue();
		} catch (Exception e) {
			if ( str.indexOf('@') != -1)
				email = str;
			else
				other = str;	

		}	

		}
	}
		

	HSob() {
		name="noname";
		score=0;
	}

	public String toDataString() {
	
	return name + "&" + score + "&" + email + "&" + other;
	}
	}
