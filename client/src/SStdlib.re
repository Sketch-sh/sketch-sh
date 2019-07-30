module ImmutableArray = {
  type t('a) = array('a);

  [@bs.send] external _push_concat: (t('a), t('a), 'a) => t('a) = "concat";
  let push = (arr, new_entry) => [||]->_push_concat(arr, new_entry);

  [@bs.send]
  external _unshift_concat: (t('a), 'a, t('a)) => t('a) = "concat";
  let unshift = (arr, new_entry) => [||]->_unshift_concat(new_entry, arr);
};

module Promise = {
  external unsafe_reject: string => 'a = "%identity";
};
