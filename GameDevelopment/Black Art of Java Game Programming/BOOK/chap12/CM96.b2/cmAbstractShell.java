abstract class cmAbstractShell extends cmAbstractMovingScenery{
   
	protected cmAbstractShell (fWorld w, fPoint3d origin, fAngle3d agl,
      fPoint3d dpos, fAngle3d dagl, double randomSpread,
      double randomRotation, double lifeTime0)
	{
      super(w,origin,agl,
         new fPoint3d(dpos.x+fWorld.rand(-randomSpread,randomSpread),
                      dpos.y+fWorld.rand(-randomSpread,randomSpread),
                      dpos.z+fWorld.rand(-randomSpread,randomSpread)),
		   new fAngle3d(dagl.x+fWorld.rand(-randomRotation,randomRotation),
                      dagl.y+fWorld.rand(-randomRotation,randomRotation),
                      dagl.z+fWorld.rand(-randomRotation,randomRotation)));
      lifeTime = lifeTime0;
	}

   public void update (double dt) {
      super.update(dt);
		
      fPoint3d v=getdPosition();
      v.y+=((cmWorld)getWorld()).gravity*dt;
      setdPosition(v);
		
      if( (getPosition().y<0) || (getAge()>lifeTime) ){
         die();
      }
   }

   //-- class constants
   protected static fPolyhedron ourDefaultPolyhedron;
   protected double lifeTime;
}

