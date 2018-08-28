Modules.require("./Notify.css");
open Utils;

let base = (~className, message) =>
  ButterToast.(
    make(
      ToastOption.make((~toastId, ~dismiss) =>
        <div key=toastId className=(Cn.make(["Notify", className]))>
          <button className="Notify__delete" onClick=(_ => dismiss()) />
          message->str
        </div>
      ),
    )
  );

let info = base(~className="Notify--info");
let success = base(~className="Notify--success");
let error = base(~className="Notify--error");
