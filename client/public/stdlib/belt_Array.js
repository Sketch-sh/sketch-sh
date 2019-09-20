'use strict';

var Curry = require("./curry.js");
var Js_math = require("./js_math.js");
var Caml_option = require("./caml_option.js");
var Caml_primitive = require("./caml_primitive.js");

function get(arr, i) {
  if (i >= 0 && i < arr.length) {
    return Caml_option.some(arr[i]);
  }
  
}

function getExn(arr, i) {
  if (!(i >= 0 && i < arr.length)) {
    throw new Error("File \"belt_Array.ml\", line 25, characters 6-12");
  }
  return arr[i];
}

function set(arr, i, v) {
  if (i >= 0 && i < arr.length) {
    arr[i] = v;
    return true;
  } else {
    return false;
  }
}

function setExn(arr, i, v) {
  if (!(i >= 0 && i < arr.length)) {
    throw new Error("File \"belt_Array.ml\", line 31, characters 4-10");
  }
  arr[i] = v;
  return /* () */0;
}

function swapUnsafe(xs, i, j) {
  var tmp = xs[i];
  xs[i] = xs[j];
  xs[j] = tmp;
  return /* () */0;
}

function shuffleInPlace(xs) {
  var len = xs.length;
  for(var i = 0 ,i_finish = len - 1 | 0; i <= i_finish; ++i){
    swapUnsafe(xs, i, Js_math.random_int(i, len));
  }
  return /* () */0;
}

function shuffle(xs) {
  var result = xs.slice(0);
  shuffleInPlace(result);
  return result;
}

function reverseInPlace(xs) {
  var len = xs.length;
  var xs$1 = xs;
  var ofs = 0;
  var len$1 = len;
  for(var i = 0 ,i_finish = (len$1 / 2 | 0) - 1 | 0; i <= i_finish; ++i){
    swapUnsafe(xs$1, ofs + i | 0, ((ofs + len$1 | 0) - i | 0) - 1 | 0);
  }
  return /* () */0;
}

function reverse(xs) {
  var len = xs.length;
  var result = new Array(len);
  for(var i = 0 ,i_finish = len - 1 | 0; i <= i_finish; ++i){
    result[i] = xs[(len - 1 | 0) - i | 0];
  }
  return result;
}

function make(l, f) {
  if (l <= 0) {
    return /* array */[];
  } else {
    var res = new Array(l);
    for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
      res[i] = f;
    }
    return res;
  }
}

function makeByU(l, f) {
  if (l <= 0) {
    return /* array */[];
  } else {
    var res = new Array(l);
    for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
      res[i] = f(i);
    }
    return res;
  }
}

function makeBy(l, f) {
  return makeByU(l, Curry.__1(f));
}

function makeByAndShuffleU(l, f) {
  var u = makeByU(l, f);
  shuffleInPlace(u);
  return u;
}

function makeByAndShuffle(l, f) {
  return makeByAndShuffleU(l, Curry.__1(f));
}

function range(start, finish) {
  var cut = finish - start | 0;
  if (cut < 0) {
    return /* array */[];
  } else {
    var arr = new Array(cut + 1 | 0);
    for(var i = 0; i <= cut; ++i){
      arr[i] = start + i | 0;
    }
    return arr;
  }
}

function rangeBy(start, finish, step) {
  var cut = finish - start | 0;
  if (cut < 0 || step <= 0) {
    return /* array */[];
  } else {
    var nb = (cut / step | 0) + 1 | 0;
    var arr = new Array(nb);
    var cur = start;
    for(var i = 0 ,i_finish = nb - 1 | 0; i <= i_finish; ++i){
      arr[i] = cur;
      cur = cur + step | 0;
    }
    return arr;
  }
}

function zip(xs, ys) {
  var lenx = xs.length;
  var leny = ys.length;
  var len = lenx < leny ? lenx : leny;
  var s = new Array(len);
  for(var i = 0 ,i_finish = len - 1 | 0; i <= i_finish; ++i){
    s[i] = /* tuple */[
      xs[i],
      ys[i]
    ];
  }
  return s;
}

function zipByU(xs, ys, f) {
  var lenx = xs.length;
  var leny = ys.length;
  var len = lenx < leny ? lenx : leny;
  var s = new Array(len);
  for(var i = 0 ,i_finish = len - 1 | 0; i <= i_finish; ++i){
    s[i] = f(xs[i], ys[i]);
  }
  return s;
}

function zipBy(xs, ys, f) {
  return zipByU(xs, ys, Curry.__2(f));
}

function concat(a1, a2) {
  var l1 = a1.length;
  var l2 = a2.length;
  var a1a2 = new Array(l1 + l2 | 0);
  for(var i = 0 ,i_finish = l1 - 1 | 0; i <= i_finish; ++i){
    a1a2[i] = a1[i];
  }
  for(var i$1 = 0 ,i_finish$1 = l2 - 1 | 0; i$1 <= i_finish$1; ++i$1){
    a1a2[l1 + i$1 | 0] = a2[i$1];
  }
  return a1a2;
}

function concatMany(arrs) {
  var lenArrs = arrs.length;
  var totalLen = 0;
  for(var i = 0 ,i_finish = lenArrs - 1 | 0; i <= i_finish; ++i){
    totalLen = totalLen + arrs[i].length | 0;
  }
  var result = new Array(totalLen);
  totalLen = 0;
  for(var j = 0 ,j_finish = lenArrs - 1 | 0; j <= j_finish; ++j){
    var cur = arrs[j];
    for(var k = 0 ,k_finish = cur.length - 1 | 0; k <= k_finish; ++k){
      result[totalLen] = cur[k];
      totalLen = totalLen + 1 | 0;
    }
  }
  return result;
}

function slice(a, offset, len) {
  if (len <= 0) {
    return /* array */[];
  } else {
    var lena = a.length;
    var ofs = offset < 0 ? Caml_primitive.caml_int_max(lena + offset | 0, 0) : offset;
    var hasLen = lena - ofs | 0;
    var copyLength = hasLen < len ? hasLen : len;
    if (copyLength <= 0) {
      return /* array */[];
    } else {
      var result = new Array(copyLength);
      for(var i = 0 ,i_finish = copyLength - 1 | 0; i <= i_finish; ++i){
        result[i] = a[ofs + i | 0];
      }
      return result;
    }
  }
}

function sliceToEnd(a, offset) {
  var lena = a.length;
  var ofs = offset < 0 ? Caml_primitive.caml_int_max(lena + offset | 0, 0) : offset;
  var len = lena - ofs | 0;
  var result = new Array(len);
  for(var i = 0 ,i_finish = len - 1 | 0; i <= i_finish; ++i){
    result[i] = a[ofs + i | 0];
  }
  return result;
}

function fill(a, offset, len, v) {
  if (len > 0) {
    var lena = a.length;
    var ofs = offset < 0 ? Caml_primitive.caml_int_max(lena + offset | 0, 0) : offset;
    var hasLen = lena - ofs | 0;
    var fillLength = hasLen < len ? hasLen : len;
    if (fillLength > 0) {
      for(var i = ofs ,i_finish = (ofs + fillLength | 0) - 1 | 0; i <= i_finish; ++i){
        a[i] = v;
      }
      return /* () */0;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

function blitUnsafe(a1, srcofs1, a2, srcofs2, blitLength) {
  if (srcofs2 <= srcofs1) {
    for(var j = 0 ,j_finish = blitLength - 1 | 0; j <= j_finish; ++j){
      a2[j + srcofs2 | 0] = a1[j + srcofs1 | 0];
    }
    return /* () */0;
  } else {
    for(var j$1 = blitLength - 1 | 0; j$1 >= 0; --j$1){
      a2[j$1 + srcofs2 | 0] = a1[j$1 + srcofs1 | 0];
    }
    return /* () */0;
  }
}

function blit(a1, ofs1, a2, ofs2, len) {
  var lena1 = a1.length;
  var lena2 = a2.length;
  var srcofs1 = ofs1 < 0 ? Caml_primitive.caml_int_max(lena1 + ofs1 | 0, 0) : ofs1;
  var srcofs2 = ofs2 < 0 ? Caml_primitive.caml_int_max(lena2 + ofs2 | 0, 0) : ofs2;
  var blitLength = Caml_primitive.caml_int_min(len, Caml_primitive.caml_int_min(lena1 - srcofs1 | 0, lena2 - srcofs2 | 0));
  if (srcofs2 <= srcofs1) {
    for(var j = 0 ,j_finish = blitLength - 1 | 0; j <= j_finish; ++j){
      a2[j + srcofs2 | 0] = a1[j + srcofs1 | 0];
    }
    return /* () */0;
  } else {
    for(var j$1 = blitLength - 1 | 0; j$1 >= 0; --j$1){
      a2[j$1 + srcofs2 | 0] = a1[j$1 + srcofs1 | 0];
    }
    return /* () */0;
  }
}

function forEachU(a, f) {
  for(var i = 0 ,i_finish = a.length - 1 | 0; i <= i_finish; ++i){
    f(a[i]);
  }
  return /* () */0;
}

function forEach(a, f) {
  return forEachU(a, Curry.__1(f));
}

function mapU(a, f) {
  var l = a.length;
  var r = new Array(l);
  for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
    r[i] = f(a[i]);
  }
  return r;
}

function map(a, f) {
  return mapU(a, Curry.__1(f));
}

function getByU(a, p) {
  var l = a.length;
  var i = 0;
  var r = undefined;
  while(r === undefined && i < l) {
    var v = a[i];
    if (p(v)) {
      r = Caml_option.some(v);
    }
    i = i + 1 | 0;
  };
  return r;
}

function getBy(a, p) {
  return getByU(a, Curry.__1(p));
}

function getIndexByU(a, p) {
  var l = a.length;
  var i = 0;
  var r = undefined;
  while(r === undefined && i < l) {
    var v = a[i];
    if (p(v)) {
      r = i;
    }
    i = i + 1 | 0;
  };
  return r;
}

function getIndexBy(a, p) {
  return getIndexByU(a, Curry.__1(p));
}

function keepU(a, f) {
  var l = a.length;
  var r = new Array(l);
  var j = 0;
  for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
    var v = a[i];
    if (f(v)) {
      r[j] = v;
      j = j + 1 | 0;
    }
    
  }
  r.length = j;
  return r;
}

function keep(a, f) {
  return keepU(a, Curry.__1(f));
}

function keepWithIndexU(a, f) {
  var l = a.length;
  var r = new Array(l);
  var j = 0;
  for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
    var v = a[i];
    if (f(v, i)) {
      r[j] = v;
      j = j + 1 | 0;
    }
    
  }
  r.length = j;
  return r;
}

function keepWithIndex(a, f) {
  return keepWithIndexU(a, Curry.__2(f));
}

function keepMapU(a, f) {
  var l = a.length;
  var r = new Array(l);
  var j = 0;
  for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
    var v = a[i];
    var match = f(v);
    if (match !== undefined) {
      r[j] = Caml_option.valFromOption(match);
      j = j + 1 | 0;
    }
    
  }
  r.length = j;
  return r;
}

function keepMap(a, f) {
  return keepMapU(a, Curry.__1(f));
}

function forEachWithIndexU(a, f) {
  for(var i = 0 ,i_finish = a.length - 1 | 0; i <= i_finish; ++i){
    f(i, a[i]);
  }
  return /* () */0;
}

function forEachWithIndex(a, f) {
  return forEachWithIndexU(a, Curry.__2(f));
}

function mapWithIndexU(a, f) {
  var l = a.length;
  var r = new Array(l);
  for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
    r[i] = f(i, a[i]);
  }
  return r;
}

function mapWithIndex(a, f) {
  return mapWithIndexU(a, Curry.__2(f));
}

function reduceU(a, x, f) {
  var r = x;
  for(var i = 0 ,i_finish = a.length - 1 | 0; i <= i_finish; ++i){
    r = f(r, a[i]);
  }
  return r;
}

function reduce(a, x, f) {
  return reduceU(a, x, Curry.__2(f));
}

function reduceReverseU(a, x, f) {
  var r = x;
  for(var i = a.length - 1 | 0; i >= 0; --i){
    r = f(r, a[i]);
  }
  return r;
}

function reduceReverse(a, x, f) {
  return reduceReverseU(a, x, Curry.__2(f));
}

function reduceReverse2U(a, b, x, f) {
  var r = x;
  var len = Caml_primitive.caml_int_min(a.length, b.length);
  for(var i = len - 1 | 0; i >= 0; --i){
    r = f(r, a[i], b[i]);
  }
  return r;
}

function reduceReverse2(a, b, x, f) {
  return reduceReverse2U(a, b, x, Curry.__3(f));
}

function reduceWithIndexU(a, x, f) {
  var r = x;
  for(var i = 0 ,i_finish = a.length - 1 | 0; i <= i_finish; ++i){
    r = f(r, a[i], i);
  }
  return r;
}

function reduceWithIndex(a, x, f) {
  return reduceWithIndexU(a, x, Curry.__3(f));
}

function everyU(arr, b) {
  var len = arr.length;
  var arr$1 = arr;
  var _i = 0;
  var b$1 = b;
  var len$1 = len;
  while(true) {
    var i = _i;
    if (i === len$1) {
      return true;
    } else if (b$1(arr$1[i])) {
      _i = i + 1 | 0;
      continue ;
    } else {
      return false;
    }
  };
}

function every(arr, f) {
  return everyU(arr, Curry.__1(f));
}

function someU(arr, b) {
  var len = arr.length;
  var arr$1 = arr;
  var _i = 0;
  var b$1 = b;
  var len$1 = len;
  while(true) {
    var i = _i;
    if (i === len$1) {
      return false;
    } else if (b$1(arr$1[i])) {
      return true;
    } else {
      _i = i + 1 | 0;
      continue ;
    }
  };
}

function some(arr, f) {
  return someU(arr, Curry.__1(f));
}

function everyAux2(arr1, arr2, _i, b, len) {
  while(true) {
    var i = _i;
    if (i === len) {
      return true;
    } else if (b(arr1[i], arr2[i])) {
      _i = i + 1 | 0;
      continue ;
    } else {
      return false;
    }
  };
}

function every2U(a, b, p) {
  return everyAux2(a, b, 0, p, Caml_primitive.caml_int_min(a.length, b.length));
}

function every2(a, b, p) {
  return every2U(a, b, Curry.__2(p));
}

function some2U(a, b, p) {
  var arr1 = a;
  var arr2 = b;
  var _i = 0;
  var b$1 = p;
  var len = Caml_primitive.caml_int_min(a.length, b.length);
  while(true) {
    var i = _i;
    if (i === len) {
      return false;
    } else if (b$1(arr1[i], arr2[i])) {
      return true;
    } else {
      _i = i + 1 | 0;
      continue ;
    }
  };
}

function some2(a, b, p) {
  return some2U(a, b, Curry.__2(p));
}

function eqU(a, b, p) {
  var lena = a.length;
  var lenb = b.length;
  if (lena === lenb) {
    return everyAux2(a, b, 0, p, lena);
  } else {
    return false;
  }
}

function eq(a, b, p) {
  return eqU(a, b, Curry.__2(p));
}

function cmpU(a, b, p) {
  var lena = a.length;
  var lenb = b.length;
  if (lena > lenb) {
    return 1;
  } else if (lena < lenb) {
    return -1;
  } else {
    var arr1 = a;
    var arr2 = b;
    var _i = 0;
    var b$1 = p;
    var len = lena;
    while(true) {
      var i = _i;
      if (i === len) {
        return 0;
      } else {
        var c = b$1(arr1[i], arr2[i]);
        if (c === 0) {
          _i = i + 1 | 0;
          continue ;
        } else {
          return c;
        }
      }
    };
  }
}

function cmp(a, b, p) {
  return cmpU(a, b, Curry.__2(p));
}

function partitionU(a, f) {
  var l = a.length;
  var i = 0;
  var j = 0;
  var a1 = new Array(l);
  var a2 = new Array(l);
  for(var ii = 0 ,ii_finish = l - 1 | 0; ii <= ii_finish; ++ii){
    var v = a[ii];
    if (f(v)) {
      a1[i] = v;
      i = i + 1 | 0;
    } else {
      a2[j] = v;
      j = j + 1 | 0;
    }
  }
  a1.length = i;
  a2.length = j;
  return /* tuple */[
          a1,
          a2
        ];
}

function partition(a, f) {
  return partitionU(a, Curry.__1(f));
}

function unzip(a) {
  var l = a.length;
  var a1 = new Array(l);
  var a2 = new Array(l);
  for(var i = 0 ,i_finish = l - 1 | 0; i <= i_finish; ++i){
    var match = a[i];
    a1[i] = match[0];
    a2[i] = match[1];
  }
  return /* tuple */[
          a1,
          a2
        ];
}

exports.get = get;
exports.getExn = getExn;
exports.set = set;
exports.setExn = setExn;
exports.shuffleInPlace = shuffleInPlace;
exports.shuffle = shuffle;
exports.reverseInPlace = reverseInPlace;
exports.reverse = reverse;
exports.make = make;
exports.range = range;
exports.rangeBy = rangeBy;
exports.makeByU = makeByU;
exports.makeBy = makeBy;
exports.makeByAndShuffleU = makeByAndShuffleU;
exports.makeByAndShuffle = makeByAndShuffle;
exports.zip = zip;
exports.zipByU = zipByU;
exports.zipBy = zipBy;
exports.unzip = unzip;
exports.concat = concat;
exports.concatMany = concatMany;
exports.slice = slice;
exports.sliceToEnd = sliceToEnd;
exports.fill = fill;
exports.blit = blit;
exports.blitUnsafe = blitUnsafe;
exports.forEachU = forEachU;
exports.forEach = forEach;
exports.mapU = mapU;
exports.map = map;
exports.getByU = getByU;
exports.getBy = getBy;
exports.getIndexByU = getIndexByU;
exports.getIndexBy = getIndexBy;
exports.keepU = keepU;
exports.keep = keep;
exports.keepWithIndexU = keepWithIndexU;
exports.keepWithIndex = keepWithIndex;
exports.keepMapU = keepMapU;
exports.keepMap = keepMap;
exports.forEachWithIndexU = forEachWithIndexU;
exports.forEachWithIndex = forEachWithIndex;
exports.mapWithIndexU = mapWithIndexU;
exports.mapWithIndex = mapWithIndex;
exports.partitionU = partitionU;
exports.partition = partition;
exports.reduceU = reduceU;
exports.reduce = reduce;
exports.reduceReverseU = reduceReverseU;
exports.reduceReverse = reduceReverse;
exports.reduceReverse2U = reduceReverse2U;
exports.reduceReverse2 = reduceReverse2;
exports.reduceWithIndexU = reduceWithIndexU;
exports.reduceWithIndex = reduceWithIndex;
exports.someU = someU;
exports.some = some;
exports.everyU = everyU;
exports.every = every;
exports.every2U = every2U;
exports.every2 = every2;
exports.some2U = some2U;
exports.some2 = some2;
exports.cmpU = cmpU;
exports.cmp = cmp;
exports.eqU = eqU;
exports.eq = eq;
/* No side effect */
