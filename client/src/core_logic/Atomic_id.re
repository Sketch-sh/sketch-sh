type t = int;

let counter = ref(0);

let make = () => {
  let new_int = counter^ + 1;
  counter := new_int;
  new_int;
};

let to_string = string_of_int;

let compare = Pervasives.compare;
