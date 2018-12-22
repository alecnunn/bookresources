import  java.net.*;
import  java.io.*;
import  java.applet.*;

class cmWorld extends fWorld{
	cmWorld (Applet app) {
		super(app,-500,-500,1000,20);
	   app.showStatus(" Creating CombatMachines96 world.");

      //-- the front wall
      new cmGenericPillar(this,-410,-300,new fAngle3d(0,0,0),2,3,12);
		for(int x=-400;x<=400;x+=20){
		   if(x==0){
		      new cmMainGate(this,new fPoint3d(x,0,-300), new fAngle3d());
            new cmGenericPillar(this,x+10,-300,new fAngle3d(0,0,0),2,3,12);
		   }else{
            new cmGenericWall(this,x,-300,new fAngle3d(0,0,0),8,2,10);
            new cmGenericPillar(this,x+10,-300,new fAngle3d(0,0,0),2,3,12);
		   }
		}		
      //-- the right wall
      new cmGenericPillar(this,-414,-295,new fAngle3d(0,Math.PI/2,0),2,3,12);
		for(int y=-285;y<=400;y+=20){
		   new cmGenericWall(this,-414,y,new fAngle3d(0,Math.PI/2,0),8,2,10);
         new cmGenericPillar(this,-414,y+10,new fAngle3d(0,Math.PI/2,0),2,3,12);
		}
      //-- the left wall
      new cmGenericPillar(this,414,-295,new fAngle3d(0,-Math.PI/2,0),2,3,12);
		for(int y=-285;y<=400;y+=20){
		   new cmGenericWall(this,414,y,new fAngle3d(0,-Math.PI/2,0),8,2,10);
         new cmGenericPillar(this,414,y+10,new fAngle3d(0,-Math.PI/2,0),2,3,12);
		}
      //-- the back wall
      new cmGenericPillar(this,-410,410,new fAngle3d(0,Math.PI,0),2,3,12);
		for(int x=-400;x<=400;x+=20){
		   new cmGenericWall(this,x,410,new fAngle3d(0,Math.PI,0),8,2,10);
         new cmGenericPillar(this,x+10,410,new fAngle3d(0,Math.PI,0),2,3,12);
		}


      //-- the "South-west" area
      for (int y=-255;y<-130;y+=30){
        for (int x=385;x>40;x-=30){
            new cmGenericBuilding(this,x,y,5,5,5);
         }
      }
      for (int y=-225;y<-130;y+=30){
         new cmGenericBuilding(this,25,y,5,5,5);
      }
      new cmGenericBuilding(this,25,-255,5,5,5); // TOWER
      new cmGenericBuilding(this,25,-105,5,5,5); // TOWER
      
     
      
      
      for (int x=385;x>130;x-=30)
        new cmGenericBuilding(this,x,-75,5,5,5);
      for (int x=385;x>100;x-=30)
        new cmGenericBuilding(this,x,-105,5,5,5);

      for (int y=0;y>-50;y-=40){
         for (int x=150;x<360;x+=40){
            new cmGenericBuilding(this,x,y,10,10,10);
         }
      }
      new cmGenericBuilding(this,385,-45,5,5,5);
      new cmGenericBuilding(this,385,5,5,5,5);

      
      
      //-- the "South-east" area
      for (int x=-75;x>-100;x-=20){
         new cmGenericBuilding(this,x,-255,5,5,5);
         new cmGenericBuilding(this,x,-235,5,5,5);
         new cmGenericBuilding(this,x,-225,5,5,5);
         new cmGenericBuilding(this,x,-205,5,5,5);
         new cmGenericBuilding(this,x,-195,5,5,5);
         new cmGenericBuilding(this,x,-175,5,5,5);
         new cmGenericBuilding(this,x,-165,5,5,5);
         new cmGenericBuilding(this,x,-145,5,5,5);
         new cmGenericBuilding(this,x,-135,5,5,5);
         new cmGenericBuilding(this,x,-115,5,5,5);
      }
      for (int x=-235;x>-260;x-=20){
         new cmGenericBuilding(this,x,-255,5,5,5);
         new cmGenericBuilding(this,x,-235,5,5,5);
         new cmGenericBuilding(this,x,-225,5,5,5);
         new cmGenericBuilding(this,x,-205,5,5,5);
         new cmGenericBuilding(this,x,-195,5,5,5);
         new cmGenericBuilding(this,x,-175,5,5,5);
         new cmGenericBuilding(this,x,-165,5,5,5);
         new cmGenericBuilding(this,x,-145,5,5,5);
         new cmGenericBuilding(this,x,-135,5,5,5);
         new cmGenericBuilding(this,x,-115,5,5,5);
      }
      for (int y=-115;y<-90;y+=20){
         new cmGenericBuilding(this,-115,y,5,5,5);
         new cmGenericBuilding(this,-125,y,5,5,5);
         new cmGenericBuilding(this,-145,y,5,5,5);
         new cmGenericBuilding(this,-155,y,5,5,5);
         new cmGenericBuilding(this,-175,y,5,5,5);
         new cmGenericBuilding(this,-185,y,5,5,5);
         new cmGenericBuilding(this,-205,y,5,5,5);
         new cmGenericBuilding(this,-215,y,5,5,5);
      }
      
      new cmGenericBuilding(this,-115,-255,5,5,5);
      new cmGenericBuilding(this,-125,-255,5,5,5);
      new cmGenericBuilding(this,-145,-255,5,5,5);
      new cmGenericBuilding(this,-155,-255,5,5,5);
      new cmGenericBuilding(this,-175,-255,5,5,5);
      new cmGenericBuilding(this,-185,-255,5,5,5);
      new cmGenericBuilding(this,-205,-255,5,5,5);
      new cmGenericBuilding(this,-215,-255,5,5,5);

      new cmGenericBuilding(this,-235,-95,5,5,5);
      new cmGenericBuilding(this,-245,-95,5,5,5);
      new cmGenericBuilding(this,-255,-95,5,5,5);
      new cmGenericBuilding(this,-255,-105,5,5,5);

      for (int y=-230;y<-130;y+=90){
         for (int x=-120;x>-220;x-=30){
            new cmGenericBuilding(this,x,y,10,10,10);
         }
      }
      new cmGenericBuilding(this,-120,-170,10,10,10);
      new cmGenericBuilding(this,-120,-200,10,10,10);
      new cmGenericBuilding(this,-210,-170,10,10,10);
      new cmGenericBuilding(this,-210,-200,10,10,10);

      for(int y=-225;y<-130;y+=30){
         new cmGenericBuilding(this,-25,y,5,5,5);
      }
      new cmGenericBuilding(this,-25,-105,5,5,5); // TOWER
      new cmGenericBuilding(this,-25,-255,5,5,5); // TOWER


      for (int y=-245;y<60;y+=30){
         for (int x=-295;x>-350;x-=50){
            new cmGenericBuilding(this,x,y,5,5,5);
         }
      }
      
      for (int y=-35;y<20;y+=30){
         for (int x=-115;x>-270;x-=30){
            new cmGenericBuilding(this,x,y,5,5,5);
         }
      }
      new cmGenericBuilding(this,-375,-5,5,5,5);
      new cmGenericBuilding(this,-375,-35,5,5,5);


      //-- the "North-east" area
      //-- the left wall
      new cmGenericPillar(this,55,405,new fAngle3d(0,-Math.PI/2,0),2,3,12);
		for(int y=395;y>185;y-=20){
		   new cmGenericWall(this,55,y,new fAngle3d(0,-Math.PI/2,0),8,2,10);
         new cmGenericPillar(this,55,y-10,new fAngle3d(0,-Math.PI/2,0),2,3,12);
		}
		
      //-- the front wall
      new cmGenericPillar(this,-410,180,new fAngle3d(0,0,0),2,3,12);
		for(int x=-400;x<=50;x+=20){
		   if((x==0) || (x==-320)) {
		      new cmMainGate(this,new fPoint3d(x,0,180), new fAngle3d());
            new cmGenericPillar(this,x+10,180,new fAngle3d(0,0,0),2,3,12);
		   }else{
            new cmGenericWall(this,x,180,new fAngle3d(0,0,0),8,2,10);
            new cmGenericPillar(this,x+10,180,new fAngle3d(0,0,0),2,3,12);
		   }
		}		
		
      //-- the inner front wall
      new cmGenericPillar(this,-50,260,new fAngle3d(0,0,0),2,3,12);
		for(int x=-40;x<=50;x+=20){
		   if(x==0) {
		      new cmMainGate(this,new fPoint3d(x,0,260), new fAngle3d());
            new cmGenericPillar(this,x+10,260,new fAngle3d(0,0,0),2,3,12);
		   }else{
            new cmGenericWall(this,x,260,new fAngle3d(0,0,0),8,2,10);
            new cmGenericPillar(this,x+10,260,new fAngle3d(0,0,0),2,3,12);
		   }
		}		
		// the inner right wall
		for(int y=255;y>190;y-=20){
		   new cmGenericWall(this,-54,y,new fAngle3d(0,Math.PI/2,0),8,2,10);
         new cmGenericPillar(this,-54,y-10,new fAngle3d(0,Math.PI/2,0),2,3,12);
		}


      for (int y=215;y<260;y+=20){
         for (int x=-135;x>-240;x-=20){
            new cmGenericBuilding(this,x,y,5,5,5);
         }
      }

      new cmGenericBuilding(this,-255,215,5,5,5);
      new cmGenericBuilding(this,-255,255,5,5,5);
      new cmGenericBuilding(this,-265,245,5,5,5);
      new cmGenericBuilding(this,-275,235,5,5,5);

      for (int y=235;y<380;y+=20){
         for (int x=-355;x>-380;x-=20){
            new cmGenericBuilding(this,x,y,5,5,5);
         }
      }

      for (int y=290;y<380;y+=40){
         for (int x=-140;x>-230;x-=40){
            new cmGenericBuilding(this,x,y,10,10,10);
         }
      }

      for (int x=0;x>-90;x-=40){
         new cmGenericBuilding(this,x,360,10,10,10);
      }
      for (int x=20;x>-70;x-=40){
         new cmGenericBuilding(this,x,300,10,10,10);
      }
      new cmGenericBuilding(this,35,275,5,5,5);
      new cmGenericBuilding(this,35,285,5,5,5);
      new cmGenericBuilding(this,-35,275,5,5,5);
      new cmGenericBuilding(this,-35,285,5,5,5);

      // the middle area
      for (int y=90;y<140;y+=40){
         for (int x=30;x<80;x+=40){
            new cmGenericBuilding(this,x,y,10,10,10);
         }
      }
      for (int y=90;y<140;y+=40){
         for (int x=-30;x>-80;x-=40){
            new cmGenericBuilding(this,x,y,10,10,10);
         }
      }
      
      for (int x=-105;x>-230;x-=30){
         new cmGenericBuilding(this,x,125,5,5,5);
      }
      for (int x=-245;x>-340;x-=30){
         new cmGenericBuilding(this,x,125,5,5,5);
      }
      for (int x=-195;x>-260;x-=30){
         new cmGenericBuilding(this,x,25,5,5,5);
      }
      new cmGenericBuilding(this,-135,25,5,5,5);
      
      new cmGenericBuilding(this,-105,65,5,5,5);
      new cmGenericBuilding(this,-105,95,5,5,5);
      new cmGenericBuilding(this,-135,95,5,5,5);

      new cmGenericBuilding(this,-195,95,5,5,5);
      new cmGenericBuilding(this,-225,55,5,5,5);

      //-- the "corridor"
      for (int i=0;i<=50;i+=10){
         new cmGenericBuilding(this,-205-i,115-i,5,5,5);
         new cmGenericBuilding(this,-235-i,115-i,5,5,5);
      }
      new cmGenericBuilding(this,-265,55,5,5,5);
      new cmGenericBuilding(this,-275,45,5,5,5);
      new cmGenericBuilding(this,-285,35,5,5,5);
         
      new cmGenericBuilding(this,-305,105,5,5,5);
      new cmGenericBuilding(this,-305,85,5,5,5);
      new cmGenericBuilding(this,-335,105,5,5,5);
      new cmGenericBuilding(this,-335,85,5,5,5);
      

      //-- the circle
      new cmGenericBuilding(this,-55,-95,5,5,5);
      new cmGenericBuilding(this,-85,-85,5,5,5);
      new cmGenericBuilding(this,-105,-65,5,5,5);
      new cmGenericBuilding(this,-105,25,5,5,5);
      new cmGenericBuilding(this,-85,45,5,5,5);
      new cmGenericBuilding(this,-55,55,5,5,5);
      new cmGenericBuilding(this,-25,65,5,5,5);
 
      new cmGenericBuilding(this,55,-95,5,5,5);
      new cmGenericBuilding(this,85,-85,5,5,5);
      new cmGenericBuilding(this,105,-65,5,5,5);
      new cmGenericBuilding(this,105,25,5,5,5);
      new cmGenericBuilding(this,85,45,5,5,5);
      new cmGenericBuilding(this,55,55,5,5,5);
      new cmGenericBuilding(this,25,65,5,5,5);
      
      new cmGenericBuilding(this,115,-5,5,5,5);
      new cmGenericBuilding(this,115,-35,5,5,5);

      //-- the active player
      new cmFesseTank(this,0,-420,Math.PI);
      //-- set the active player to the glider if you wish
      theActivePlayer=new cmGlider(this,20,-400,0,Math.PI,0);

      //-- the cannons
      cmAttackBrain ab;
      
      ab=new cmAttackBrain(new cmAntiTankCannon(this,-165,-185,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-165,65,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-295,165,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-345,165,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-295,215,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-345,215,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();
      
      ab=new cmAttackBrain(new cmAntiTankCannon(this,25,225,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();
      
      ab=new cmAttackBrain(new cmAntiTankCannon(this,-25,225,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();
      
      ab=new cmAttackBrain(new cmAntiTankCannon(this,15,335,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-15,335,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();
      
      ab=new cmAttackBrain(new cmAntiTankCannon(this,-255,295,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-255,365,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmAntiTankCannon(this,-305,325,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      
/*
      ab=new cmAttackBrain(new cmFesseTank(this,0,-500,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();

      ab=new cmAttackBrain(new cmFesseTank(this,0,-360,Math.PI));
      ab.selectTarget(theActivePlayer);
      ab.attackTarget();
*/

		cmMotionBrain b=new cmMotionBrain(new cmGlider(this,-10,-370,0,0,0));
		b.gotoPosition(new fPoint3d(0,30,100),0.01);
	}

	protected void initiateClasses (Applet app) {
		cmGlider          .initiateClass(app);
		cmFesseTank       .initiateClass(app);
		cmGenericBuilding .initiateClass(app);
		cmGenericFragment .initiateClass(app);
		cmMinicannon      .initiateClass(app);
		cmMissileLauncher .initiateClass(app);
		cmGenericExplosion.initiateClass(app);
		cmBombBay         .initiateClass(app);
		cmMainGate        .initiateClass(app);
		cmGenericWall     .initiateClass(app);
		cmGenericPillar   .initiateClass(app);
		cmAntiTankCannon  .initiateClass(app);
	}

   public cmAbstractPlayer getActivePlayer(){
      return theActivePlayer;
   }

	public cmAbstractPlayer theActivePlayer;
	public static final double gravity=-10;
}

