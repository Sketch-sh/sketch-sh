Modules.import("./Notify.css");
open Utils;

[@react.component]
let base = (~className, ~sticky=false, message) =>
  ButterToast.(
    make(
      ToastOption.make(~sticky, (~toastId, ~dismiss) =>
        <div key=toastId className={Cn.make(["Notify", className])}>
          <button className="Notify__delete" onClick={_ => dismiss()} />
          message->str
        </div>
      ),
    )
  );

let info = base(~className="Notify--info");
let success = base(~className="Notify--success");
let error = base(~className="Notify--error");
