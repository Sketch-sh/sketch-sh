module Map = Belt.MutableMap.String;
let fs: Map.t(string) = Map.make();

let set = (~filename, ~content: string) => {
  fs->Map.set(filename, content);
};

let get = (~filename) => {
  fs->Map.get(filename);
};

let delete = (~filename) => {
  fs->Map.remove(filename);
};
