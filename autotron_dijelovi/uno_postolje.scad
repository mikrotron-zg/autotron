// stalak za arduino uno i ultrazvucni senzor na robotskim kolicima

flatw=65;
flatl=68;
flath=2;
bevelradius=2;

unow=53.53;
unol=68.72;

hcw=46;
hch=1.7;
holed=3.5;

pind=2.5;
pinh=3;

union(){
    difference(){
        podloga(flatw,flatl,flath,bevelradius);
        rupe([flatw/2,flatl/2.8,flath/2],flath,holed/2);
    }
    pinovi(4,pind/2,[flatw/2,-5,flath],pinh);
}

// povrsina ispod uno plocice
module podloga(flatw,flatl,flath,bevelradius){

    inw=flatw-2*bevelradius;
    inl=flatl-2*bevelradius;

    translate([bevelradius,bevelradius,0]){
        difference(){
            union(){
                cube([inw,inl,flath],false);
                cylinder(flath,bevelradius,bevelradius,false);
                translate([inw,0,0]){
                    cylinder(flath,bevelradius,bevelradius,false);
                    translate([0,inl,0]){
                        cylinder(flath,bevelradius,bevelradius,false);
                    }
                }
                translate([0,inl,0]){cylinder(flath,bevelradius,bevelradius,false);}
                
                translate([-bevelradius,0,0]){cube([bevelradius,inl,flath],false);}
                translate([inw,0,0]){cube([bevelradius,inl,flath],false);}
                translate([0,-bevelradius,0]){cube([inw,bevelradius,flath],false);}
                translate([0,inl,0]){cube([inw,bevelradius,flath],false);}
                
                translate([inw/2-hcw/2-2,inl+bevelradius,0]){cube([hcw+4,10,flath],false);}
                translate([inw/2+hcw/2,inl+6,0]){cube([2,6,20]);}
                translate([inw/2+hcw/2-2,inl+6,0]){cube([2,2,20]);}
                translate([inw/2+hcw/2-2,inl+10,0]){cube([2,2,20]);}
                translate([inw/2-2-hcw/2,inl+6,0]){cube([2,6,20]);}
                translate([inw/2-hcw/2,inl+6,0]){cube([2,2,20]);}
                translate([inw/2-hcw/2,inl+10,0]){cube([2,2,20]);}
            }
            translate([inw/2,inl/2,flath/2]){cylinder(flath+0.2,18,18,true);}
        }
    }
}

module pinovi(lift,pinr,off,pinh){
    
    startpinr=2.5*pinr;
    endpinr=2*pinr;
    pin1=[50.95,13.6,0]+off;      //uz power
    pin2=[2.82,15.11,0]+off;    //uz usb
    pin3=[46.05,66,0]+off;      // uz procesor
    pin4=[18.05,66,0]+off;
    
    translate([-unow/2,0,0]){
        translate(pin1){
            union(){
                cylinder(lift,startpinr,endpinr,false,$fn=20);
                //cylinder(lift+pinh,pinr,pinr,false,$fn=20);  //otkomentirati za uno
            }
        }
        translate(pin2){
            union(){
                cylinder(lift,startpinr,endpinr,false,$fn=20);
                cylinder(lift+pinh,pinr,pinr,false,$fn=20);
            }
        }
        translate(pin3){
            union(){
                cylinder(lift,startpinr,endpinr,false,$fn=20);
                cylinder(lift+pinh,pinr,pinr,false,$fn=20);
            }
        }
        translate(pin4){
            union(){
                cylinder(lift,startpinr,endpinr,false,$fn=20);
                cylinder(lift+pinh,pinr,pinr,false,$fn=20);
            }
        }
    }
}

module rupe(off,h,r){
    
    wsplit1=46.5;
    wsplit2=43.5;
    lsplit=7.93;
    
    translate(off){
        translate([-wsplit1/2,-lsplit/2,0]){cylinder(h+0.2,r,r,true,$fn=20);}
        translate([wsplit1/2,-lsplit/2,0]){cylinder(h+0.2,r,r,true,$fn=20);}
        translate([-wsplit2/2,lsplit/2,0]){cylinder(h+0.2,r,r,true,$fn=20);}
        translate([wsplit2/2,lsplit/2,0]){cylinder(h+0.2,r,r,true,$fn=20);}
    }
}