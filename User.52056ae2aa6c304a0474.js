(this.webpackJsonp=this.webpackJsonp||[]).push([[9],{251:function(e,t,n){},257:function(e,t,n){"use strict";n.r(t),n.d(t,"GetNotes",function(){return N}),n.d(t,"GetNotesComponent",function(){return x}),n.d(t,"getNotesQuery",function(){return w}),n.d(t,"updateQuery",function(){return S}),n.d(t,"shouldFetchMore",function(){return k}),n.d(t,"initialState",function(){return E}),n.d(t,"reducer",function(){return U}),n.d(t,"component",function(){return j}),n.d(t,"make",function(){return O});var i=n(1),r=n(0),o=n(8),a=n(19),d=n(24),u=n(20),v=n(15),s=n(21),c=n(2),l=n(5),p=n(68),f=n(17),m=n(34),g=n(175),b=n(176);n(251);var h="query getNotes($userName: String!, $limit: Int, $offset: Int)  {\nnote(where: {owner: {username: {_eq: $userName}}}, order_by: {updated_at: desc}, limit: $limit, offset: $offset)  {\nid  \ntitle  \ndate: updated_at  \n}\n\n}\n";function y(e){var t=u.d(e);if(void 0!==t){var n,i=d.c(c.d(t),"note");if(void 0!==i){var r=c.d(i),o=u.a(r);n=void 0!==o?o.map(function(e){var t=u.d(e);if(void 0!==t){var n,i=c.d(t),r=d.c(i,"id");if(void 0!==r){var o=c.d(r),v=u.e(o);n=void 0!==v?v:a.a("graphql_ppx: Expected string, got "+JSON.stringify(o))}else n=a.a("graphql_ppx: Field id on type note is missing");var s,l=d.c(i,"title");if(void 0!==l){var p=c.d(l);if(void 0!==u.b(p))s=void 0;else{var f=u.e(p);s=void 0!==f?f:a.a("graphql_ppx: Expected string, got "+JSON.stringify(p))}}else s=void 0;var m=d.c(i,"date");return{id:n,title:s,date:void 0!==m?c.d(m):a.a("graphql_ppx: Field date on type note is missing")}}return a.a("graphql_ppx: Object is not a value")}):a.a("graphql_ppx: Expected array, got "+JSON.stringify(r))}else n=a.a("graphql_ppx: Field note on type query_root is missing");return{note:n}}return a.a("graphql_ppx: Object is not a value")}function _(e,t,n,i){return{query:h,variables:d.a([["userName",e],["limit",void 0!==t?t:null],["offset",void 0!==n?n:null]]),parse:y}}var q=i.b([],[]),N=i.b(["ppx_printed_query","query","parse","make","makeWithVariables","ret_type","MT_Ret"],[h,h,y,_,function(e){var t=e.userName,n=e.limit,i=e.offset;return{query:h,variables:d.a([["userName",t],["limit",void 0!==n?n:null],["offset",void 0!==i?i:null]]),parse:y}},function(e){return i.b([],[])},q]),x=p.b([h,y]);function w(e,t,n){var i=void 0!==t?t:0;return _(e,m.f,i)}function S(e,t){var n=t.fetchMoreResult;if(void 0!==n){var i={note:v.b(e.note,n.note)};return f.a[0](e,i)}return e}function k(e,t){return e.length>=s.b(m.f,t)}function E(e){return i.c(["count"],[1])}function U(e,t){return i.e("Update",0,[i.c(["count"],[1+t[0]|0])])}var j=l.c("User");function O(e,t){return i.c(["debugName","reactClassInternal","handedOffState","willReceiveProps","didMount","didUpdate","willUnmount","willUpdate","shouldUpdate","render","initialState","retainedProps","reducer","jsElementWrapped"],[j[0],j[1],j[2],j[3],j[4],j[5],j[6],j[7],j[8],function(t){var n=w(e,void 0);return o.createElement("section",{className:"Layout__center User"},o.createElement("h1",void 0,f.j(String(e)+"'s sketches")),l.b(void 0,void 0,r.j(x[3],[c.c(n.variables),void 0,void 0,"network-only",void 0,void 0,void 0,void 0,void 0,function(n){var i=n[5],a=n[0];if("number"==typeof a)return o.createElement("div",{style:{width:"500px"}},l.b(void 0,void 0,b.a[1](void 0,void 0,[])));if(a.tag){var d=a[0],u=w(e,d.note.length);return k(d.note,t[1][0])?l.b(void 0,void 0,b.c(d.note,void 0,c.c(l.b(void 0,void 0,g.a(void 0,[]))),function(e){return r.c(i,c.c(u.variables),S,0).then(function(e){return r.a(t[3],0),Promise.resolve(0)}),0},[])):l.b(void 0,void 0,b.c(d.note,void 0,c.c(l.b(void 0,void 0,g.a(void 0,[]))),void 0,[]))}return f.j(a[0].message)}])))},E,j[11],U,j[13]])}}}]);