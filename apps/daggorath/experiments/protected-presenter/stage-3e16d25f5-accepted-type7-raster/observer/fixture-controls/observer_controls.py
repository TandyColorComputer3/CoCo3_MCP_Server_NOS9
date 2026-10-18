#!/usr/bin/env python3
"""Offline transliteration smoke tests; actual Lua tests run in MAME startup."""
from dataclasses import dataclass

PAL = [(0,0,0),(85,85,85),(170,170,170),(255,255,255)]
@dataclass
class Result:
    ok: bool
    reason: str = ''
    transform: tuple | None = None
    colors: tuple | None = None
    crop: tuple | None = None

def make_frame(w,h,sx=1,sy=1,ox=0,oy=0,bands=(0,1,2,3),width_bad=None,height_bad=None,shift=0,markers=True):
    a=[PAL[0]]*(w*h)
    def rect(lx,ly,rw,rh,color):
        for y in range(oy+ly*sy,oy+(ly+rh)*sy):
            for x in range(ox+lx*sx,ox+(lx+rw)*sx):
                assert 0<=x<w and 0<=y<h
                a[y*w+x]=PAL[color]
    if markers:
        rect(8,16,8,8,1); rect(24,16,16,8,2); rect(56,64,6,8,3)
    rect(63,63,34,10,3)
    for i,c in enumerate(bands):
        rw=width_bad[1] if width_bad and width_bad[0]==i else 8
        rh=height_bad[1] if height_bad and height_bad[0]==i else 8
        rect(64+i*8+shift,64,rw,rh,c)
    return a

def rect_is(a,w,h,x,y,rw,rh,color):
    return x>=0 and y>=0 and x+rw<=w and y+rh<=h and all(
        a[yy*w+xx]==color for yy in range(y,y+rh) for xx in range(x,x+rw))

def classify(a,w,h):
    if w<1 or h<1 or len(a)!=w*h: return Result(False,'pixel_length_mismatch')
    candidates=[]
    for sy in range(1,5):
      for sx in range(1,5):
       for y in range(h-8*sy+1):
        for x in range(w-8*sx+1):
         if a[y*w+x]==PAL[1] and rect_is(a,w,h,x,y,8*sx,8*sy,PAL[1]) \
            and rect_is(a,w,h,x+16*sx,y,16*sx,8*sy,PAL[2]) \
            and rect_is(a,w,h,x+48*sx,y+48*sy,6*sx,8*sy,PAL[3]):
          candidates.append((sx,sy,x-8*sx,y-16*sy,x,y))
    if len(candidates)!=1: return Result(False,'fiducials_missing_or_ambiguous')
    sx,sy,ox,oy,mx,my=candidates[0]
    px,py=ox+63*sx,oy+63*sy
    if not all((rect_is(a,w,h,px,py,34*sx,sy,PAL[3]),
      rect_is(a,w,h,px,py+9*sy,34*sx,sy,PAL[3]),
      rect_is(a,w,h,px,py,sx,10*sy,PAL[3]),
      rect_is(a,w,h,px+33*sx,py,sx,10*sy,PAL[3]))):
      return Result(False,'frame_mismatch')
    observed=[]; raw=[]
    for i in range(4):
      x0=ox+64*sx+i*8*sx; y0=oy+64*sy
      block=[a[y*w+x] for y in range(y0,y0+8*sy) for x in range(x0,x0+8*sx)]
      expected=PAL[i]
      if any(c!=expected for c in block): return Result(False,'band_color_order_or_geometry_mismatch')
      observed.append(block[0]); raw.extend(block)
    return Result(True,transform=(sx,sy,ox,oy),colors=tuple(observed),crop=tuple(raw))

def stable_step(result, state):
    if not result.ok: state[:]=[None,0]; return False
    key=(result.transform,result.crop)
    if key==state[0]: state[1]+=1
    else: state[:]=[key,1]
    return state[1]>=3

def check(name, result, want):
    assert result.ok is want, f'{name}: expected {want}, got {result}'

base=make_frame(192,144,1,1,7,9)
r=classify(base,192,144); check('correct 1x',r,True)
assert r.transform==(1,1,7,9) and r.colors==tuple(PAL)
check('reversed',classify(make_frame(192,144,1,1,7,9,(3,2,1,0)),192,144),False)
check('missing',classify(make_frame(192,144,1,1,7,9,(0,1,2,2)),192,144),False)
check('duplicate',classify(make_frame(192,144,1,1,7,9,(0,1,1,3)),192,144),False)
check('shifted',classify(make_frame(192,144,1,1,7,9,shift=1),192,144),False)
check('wrong width',classify(make_frame(192,144,1,1,7,9,width_bad=(1,7)),192,144),False)
check('wrong height',classify(make_frame(192,144,1,1,7,9,height_bad=(1,7)),192,144),False)
check('no fiducials',classify(make_frame(192,144,1,1,7,9,markers=False),192,144),False)
check('uniform image',classify([PAL[0]]*(192*144),192,144),False)
check('truncated',classify(base[:-1],192,144),False)
alt=classify(make_frame(220,180,2,2,11,13),220,180); check('alternative dimensions/scale',alt,True)
assert alt.transform==(2,2,11,13)
state=[None,0]
assert not stable_step(alt,state)
alt_moved=classify(make_frame(220,180,2,2,12,13),220,180)
assert not stable_step(alt_moved,state)
assert not stable_step(alt,state)
assert not stable_step(alt,state)
assert stable_step(alt,state)
print('PASS: offline transliteration controls (16 positive/negative/stability assertions)')
print('LIMIT: this is not an execution of the MAME Lua implementation; observer startup runs equivalent Lua self-tests before guest input.')
