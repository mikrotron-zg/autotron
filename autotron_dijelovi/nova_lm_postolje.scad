// stalak za itead lm motor kontroler i croduino novu na robotskim kolicima

flatw=55;
flatl=88;
flath=2;
bevelradius=2;

unow=53.53;
unol=68.72;

nova_l=50;
nova_w=23.7;
moto_a=50;
moto_b=55.1;
moto_hole_a=40;
moto_hole_b=45;

hcw=46;
hch=1.7;
holed=3.5;

pind=2.5;
pinh=3;

union(){
    difference(){
        podloga(flatw,flatl,flath,bevelradius);
        rupe([flatw/2,flatl/2.5,flath/2],flath,holed/2);
    }
    pinovi(4,pind/2,[(flatw-moto_hole_a)/2,5,flath-0.2],pinh);
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
                
                //translate([inw/2-hcw/2-2,inl+bevelradius,0]){cube([hcw+4,10,flath],false)};
                translate([inw/2,inl-2-nova_w/2,flath+5]){
                    difference(){
                        cube([nova_l,nova_w+4,10],true);
                        cube([nova_l+2,nova_w,11],true);
                        cube([10,nova_w+6,11],true);
                    }
                }

            }
            translate([inw/2,45/2+10,flath/2]){cube([25,45,10],true);}
        }
    }
}

module pinovi(lift,pinr,off,pinh){
    
    startpinr=2.5*pinr;
    endpinr=2*pinr;
    pin1=[0,0,0];
    pin2=[moto_hole_a,0,0];
    pin3=[0,moto_hole_b,0];
    pin4=[moto_hole_a,moto_hole_b,0];
    
    translate(off){
        translate(pin1){
            union(){
                cylinder(lift,startpinr,endpinr,false,$fn=20);
                cylinder(lift+pinh,pinr,pinr,false,$fn=20);
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