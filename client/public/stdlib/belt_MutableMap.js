'use strict';

var Curry = require("./curry.js");
var Caml_option = require("./caml_option.js");
var Belt_internalAVLtree = require("./belt_internalAVLtree.js");

function removeMutateAux(nt, x, cmp) {
  var k = nt.key;
  var c = cmp(x, k);
  if (c === 0) {
    var l = nt.left;
    var r = nt.right;
    if (l !== null) {
      if (r !== null) {
        nt.right = Belt_internalAVLtree.removeMinAuxWithRootMutate(nt, r);
        return Belt_internalAVLtree.balMutate(nt);
      } else {
        return l;
      }
    } else if (r !== null) {
      return r;
    } else {
      return l;
    }
  } else if (c < 0) {
    var match = nt.left;
    if (match !== null) {
      nt.left = removeMutateAux(match, x, cmp);
      return Belt_internalAVLtree.balMutate(nt);
    } else {
      return nt;
    }
  } else {
    var match$1 = nt.right;
    if (match$1 !== null) {
      nt.right = removeMutateAux(match$1, x, cmp);
      return Belt_internalAVLtree.balMutate(nt);
    } else {
      return nt;
    }
  }
}

function remove(d, k) {
  var oldRoot = d.data;
  if (oldRoot !== null) {
    var newRoot = removeMutateAux(oldRoot, k, d.cmp);
    if (newRoot !== oldRoot) {
      d.data = newRoot;
      return /* () */0;
    } else {
      return 0;
    }
  } else {
    return /* () */0;
  }
}

function removeArrayMutateAux(_t, xs, _i, len, cmp) {
  while(true) {
    var i = _i;
    var t = _t;
    if (i < len) {
      var ele = xs[i];
      var u = removeMutateAux(t, ele, cmp);
      if (u !== null) {
        _i = i + 1 | 0;
        _t = u;
        continue ;
      } else {
        return Belt_internalAVLtree.empty;
      }
    } else {
      return t;
    }
  };
}

function removeMany(d, xs) {
  var oldRoot = d.data;
  if (oldRoot !== null) {
    var len = xs.length;
    var newRoot = removeArrayMutateAux(oldRoot, xs, 0, len, d.cmp);
    if (newRoot !== oldRoot) {
      d.data = newRoot;
      return /* () */0;
    } else {
      return 0;
    }
  } else {
    return /* () */0;
  }
}

function updateDone(t, x, f, cmp) {
  if (t !== null) {
    var k = t.key;
    var c = cmp(x, k);
    if (c === 0) {
      var match = f(Caml_option.some(t.value));
      if (match !== undefined) {
        t.value = Caml_option.valFromOption(match);
        return t;
      } else {
        var l = t.left;
        var r = t.right;
        if (l !== null) {
          if (r !== null) {
            t.right = Belt_internalAVLtree.removeMinAuxWithRootMutate(t, r);
            return Belt_internalAVLtree.balMutate(t);
          } else {
            return l;
          }
        } else if (r !== null) {
          return r;
        } else {
          return l;
        }
      }
    } else {
      var l$1 = t.left;
      var r$1 = t.right;
      if (c < 0) {
        var ll = updateDone(l$1, x, f, cmp);
        t.left = ll;
      } else {
        t.right = updateDone(r$1, x, f, cmp);
      }
      return Belt_internalAVLtree.balMutate(t);
    }
  } else {
    var match$1 = f(undefined);
    if (match$1 !== undefined) {
      return Belt_internalAVLtree.singleton(x, Caml_option.valFromOption(match$1));
    } else {
      return t;
    }
  }
}

function updateU(t, x, f) {
  var oldRoot = t.data;
  var newRoot = updateDone(oldRoot, x, f, t.cmp);
  if (newRoot !== oldRoot) {
    t.data = newRoot;
    return /* () */0;
  } else {
    return 0;
  }
}

function update(t, x, f) {
  return updateU(t, x, Curry.__1(f));
}

function make(id) {
  return {
          cmp: id[/* cmp */0],
          data: Belt_internalAVLtree.empty
        };
}

function clear(m) {
  m.data = Belt_internalAVLtree.empty;
  return /* () */0;
}

function isEmpty(d) {
  var x = d.data;
  return x === null;
}

function minKey(m) {
  return Belt_internalAVLtree.minKey(m.data);
}

function minKeyUndefined(m) {
  return Belt_internalAVLtree.minKeyUndefined(m.data);
}

function maxKey(m) {
  return Belt_internalAVLtree.maxKey(m.data);
}

function maxKeyUndefined(m) {
  return Belt_internalAVLtree.maxKeyUndefined(m.data);
}

function minimum(m) {
  return Belt_internalAVLtree.minimum(m.data);
}

function minUndefined(m) {
  return Belt_internalAVLtree.minUndefined(m.data);
}

function maximum(m) {
  return Belt_internalAVLtree.maximum(m.data);
}

function maxUndefined(m) {
  return Belt_internalAVLtree.maxUndefined(m.data);
}

function forEachU(d, f) {
  return Belt_internalAVLtree.forEachU(d.data, f);
}

function forEach(d, f) {
  return Belt_internalAVLtree.forEachU(d.data, Curry.__2(f));
}

function reduceU(d, acc, cb) {
  return Belt_internalAVLtree.reduceU(d.data, acc, cb);
}

function reduce(d, acc, cb) {
  return reduceU(d, acc, Curry.__3(cb));
}

function everyU(d, p) {
  return Belt_internalAVLtree.everyU(d.data, p);
}

function every(d, p) {
  return Belt_internalAVLtree.everyU(d.data, Curry.__2(p));
}

function someU(d, p) {
  return Belt_internalAVLtree.someU(d.data, p);
}

function some(d, p) {
  return Belt_internalAVLtree.someU(d.data, Curry.__2(p));
}

function size(d) {
  return Belt_internalAVLtree.size(d.data);
}

function toList(d) {
  return Belt_internalAVLtree.toList(d.data);
}

function toArray(d) {
  return Belt_internalAVLtree.toArray(d.data);
}

function keysToArray(d) {
  return Belt_internalAVLtree.keysToArray(d.data);
}

function valuesToArray(d) {
  return Belt_internalAVLtree.valuesToArray(d.data);
}

function checkInvariantInternal(d) {
  return Belt_internalAVLtree.checkInvariantInternal(d.data);
}

function cmpU(m1, m2, cmp) {
  return Belt_internalAVLtree.cmpU(m1.data, m2.data, m1.cmp, cmp);
}

function cmp(m1, m2, cmp$1) {
  return cmpU(m1, m2, Curry.__2(cmp$1));
}

function eqU(m1, m2, cmp) {
  return Belt_internalAVLtree.eqU(m1.data, m2.data, m1.cmp, cmp);
}

function eq(m1, m2, cmp) {
  return eqU(m1, m2, Curry.__2(cmp));
}

function mapU(m, f) {
  return {
          cmp: m.cmp,
          data: Belt_internalAVLtree.mapU(m.data, f)
        };
}

function map(m, f) {
  return mapU(m, Curry.__1(f));
}

function mapWithKeyU(m, f) {
  return {
          cmp: m.cmp,
          data: Belt_internalAVLtree.mapWithKeyU(m.data, f)
        };
}

function mapWithKey(m, f) {
  return mapWithKeyU(m, Curry.__2(f));
}

function get(m, x) {
  return Belt_internalAVLtree.get(m.data, x, m.cmp);
}

function getUndefined(m, x) {
  return Belt_internalAVLtree.getUndefined(m.data, x, m.cmp);
}

function getWithDefault(m, x, def) {
  return Belt_internalAVLtree.getWithDefault(m.data, x, def, m.cmp);
}

function getExn(m, x) {
  return Belt_internalAVLtree.getExn(m.data, x, m.cmp);
}

function has(m, x) {
  return Belt_internalAVLtree.has(m.data, x, m.cmp);
}

function fromArray(data, id) {
  var cmp = id[/* cmp */0];
  return {
          cmp: cmp,
          data: Belt_internalAVLtree.fromArray(data, cmp)
        };
}

function set(m, e, v) {
  var oldRoot = m.data;
  var newRoot = Belt_internalAVLtree.updateMutate(oldRoot, e, v, m.cmp);
  if (newRoot !== oldRoot) {
    m.data = newRoot;
    return /* () */0;
  } else {
    return 0;
  }
}

function mergeManyAux(t, xs, cmp) {
  var v = t;
  for(var i = 0 ,i_finish = xs.length - 1 | 0; i <= i_finish; ++i){
    var match = xs[i];
    v = Belt_internalAVLtree.updateMutate(v, match[0], match[1], cmp);
  }
  return v;
}

function mergeMany(d, xs) {
  var oldRoot = d.data;
  var newRoot = mergeManyAux(oldRoot, xs, d.cmp);
  if (newRoot !== oldRoot) {
    d.data = newRoot;
    return /* () */0;
  } else {
    return 0;
  }
}

var Int = 0;

var $$String = 0;

exports.Int = Int;
exports.$$String = $$String;
exports.make = make;
exports.clear = clear;
exports.isEmpty = isEmpty;
exports.has = has;
exports.cmpU = cmpU;
exports.cmp = cmp;
exports.eqU = eqU;
exports.eq = eq;
exports.forEachU = forEachU;
exports.forEach = forEach;
exports.reduceU = reduceU;
exports.reduce = reduce;
exports.everyU = everyU;
exports.every = every;
exports.someU = someU;
exports.some = some;
exports.size = size;
exports.toList = toList;
exports.toArray = toArray;
exports.fromArray = fromArray;
exports.keysToArray = keysToArray;
exports.valuesToArray = valuesToArray;
exports.minKey = minKey;
exports.minKeyUndefined = minKeyUndefined;
exports.maxKey = maxKey;
exports.maxKeyUndefined = maxKeyUndefined;
exports.minimum = minimum;
exports.minUndefined = minUndefined;
exports.maximum = maximum;
exports.maxUndefined = maxUndefined;
exports.get = get;
exports.getUndefined = getUndefined;
exports.getWithDefault = getWithDefault;
exports.getExn = getExn;
exports.checkInvariantInternal = checkInvariantInternal;
exports.remove = remove;
exports.removeMany = removeMany;
exports.set = set;
exports.updateU = updateU;
exports.update = update;
exports.mergeMany = mergeMany;
exports.mapU = mapU;
exports.map = map;
exports.mapWithKeyU = mapWithKeyU;
exports.mapWithKey = mapWithKey;
/* No side effect */
