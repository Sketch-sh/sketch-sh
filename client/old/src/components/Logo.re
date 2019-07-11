let component = ReasonReact.statelessComponent("Logo");

[@react.component]
let make = (~size=1., ~className=?, ()) =>
  ReactCompat.useRecordApi({
    ...component,
    render: _self => {
      let width = (172.327 *. size)->Js.Float.toString;
      let height = (193.867 *. size)->Js.Float.toString;
      <svg ?className width height viewBox="0 0 172.327 193.867">
        <path
          fill="#1874d5"
          d="M86.983.056C39.397.056.819 38.631.819 86.219v21.541c0 47.589 38.578 86.163 86.164 86.163h86.163V86.22c0-47.59-38.577-86.164-86.163-86.164"
        />
        <path
          fill="#FFF"
          d="M151.605 159.219h-29.918V91.604c0-19.135-15.569-34.704-34.704-34.704-19.136 0-34.705 15.569-34.705 34.704H35.524c0-28.374 23.085-51.458 51.459-51.458 28.374 0 51.458 23.084 51.458 51.458v50.86h13.164v16.755zm-50.262-51.459c0 7.932-6.429 14.36-14.36 14.36-7.93 0-14.36-6.428-14.36-14.36 0-7.93 6.43-14.36 14.36-14.36 7.931 0 14.36 6.43 14.36 14.36z"
        />
      </svg>;
    },
  });
