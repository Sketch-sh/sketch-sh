module E = Express;

let apply = router => {
  //
  // Mercury! 🚀
  //
  router->LoginMercuryEndpoint.Endpoint.use;

  //
  // Gifts! 🎁
  //
  router->GiftPurchaseEndpoint.Endpoint.use;
  router->GiftGeneratePDFEndpoint.Endpoint.use;
  router->GiftPurchaseAdminEndpoint.Endpoint.use;
  router->RedeemGiftEndpoint.Endpoint.use;
  router->PremiumStatusEndpoint.Endpoint.use;
  router->CreateGiftsForAmbassadorEndpoint.Endpoint.use;
  router->ReadGiftsForAmbassadorEndpoint.Endpoint.use;

  //
  // Instagram! 📸
  //
  router->GetInstaAuthUrlEndpoint.Endpoint.use;
  router->VerifyAuthUrlEndpoint.Endpoint.use;
  router->AllInstaEndpoint.Endpoint.use;
  router->UnlinkInstaEndpoint.Endpoint.use;
  router->UpdateInstaEndpoint.Endpoint.use;
  router->RedirectForClientEndpoint.Endpoint.use;
  router->ReImportInstaEndpoint.Endpoint.use;

  //
  // Stats! 📈
  //
  router->AppleReceiptsEndpoint.Endpoint.use;
  router->HasuraAuthEndpoint.Endpoint.use;

  //
  // Health! 🍎
  //
  router->AppleReceiptsHealthEndpoint.Endpoint.use;
  router->StatsEndpoint.Endpoint.use;
  router->Health.Endpoint.use;

  //
  // Accounts! 👤
  //
  router->DeleteUserEndpoint.Endpoint.use;
  router->CreateChocolateUserEndpoint.Endpoint.use;
  router->IsChocolateUserEndpoint.Endpoint.use;
  router->LoginPasswordEndpoint.Endpoint.use;
  router->LoginCloudkitEndpoint.Endpoint.use;
  router->LoginGoogleEndpoint.Endpoint.use;
  router->LogoutEndpoint.Endpoint.use;
  router->UsersEndpoint.Endpoint.use;
  router->UpdateUserEndpoint.Endpoint.use;

  //
  // Syncable 🚰
  //
  router->CreateSyncableEndpoint.Endpoint.use;
  router->ReadOneSyncableEndpoint.Endpoint.use;
  router->UpdateSyncableEndpoint.Endpoint.use;
  router->SyncableChangesEndpoint.Endpoint.use;

  router->UpsertAttachmentEndpoint.Endpoint.use;
  router->ReadAttachmentEndpoint.Endpoint.use;
  router->DeleteAttachmentEndpoint.Endpoint.use;

  router->UpsertNamedSyncableEndpoint.Endpoint.use;
  router->ReadNamedSyncableEndpoint.Endpoint.use;
  router->DeleteNamedSyncableEndpoint.Endpoint.use;

  //
  // Incoming Email 📬
  //
  router->GetInmailAddressEndpoint.Endpoint.use;
  router->RevokeInmailAddressEndpoint.Endpoint.use;

  //
  // OAuth 🔑
  //
  router->AuthorizeOauthEndpoint.Endpoint.use;
  router->OauthRedeemEndpoint.Endpoint.use;

  //
  // IFTTT ⚙️
  //
  router->IFTTTStatusEndpoint.Endpoint.use;
  router->IFTTTTestSetupEndpoint.Endpoint.use;
  router->IFTTTGetUserInfoEndpoint.Endpoint.use;
  router->IFTTTCreateEntryEndpoint.Endpoint.use;
  router->IFTTTCreateEntryFieldOptionsEndpoint.Endpoint.use;

  //
  // Printing 🖨
  //
  router->PrintLimitsEndpoint.Endpoint.use;
  router->PrintEstimatePriceEndpoint.Endpoint.use;
  router->PrintGetOrdersEndpoint.Endpoint.use;
  router->PrintCreateOrderEndpoint.Endpoint.use;
  router->PrintCheckOrderStatusEndpoint.Endpoint.use;
  router->PrintUpdateOrderEndpoint.Endpoint.use;
  router->PrintProcessPaymentEndpoint.Endpoint.use;
  router->PrintCancelOrderEndpoint.Endpoint.use;
  router->PrintPartnerGetReadyOrdersEndpoint.Endpoint.use;
  router->PrintPartnerGetCancelledOrdersEndpoint.Endpoint.use;
  router->PrintPartnerUpdatePrintingStatusEndpoint.Endpoint.use;

  //
  // Test! 🚧
  //
  router->CleanAccountTestEndpoint.Endpoint.use;
  router->TestGiftSetupEndpoint.Endpoint.use;

  //
  // Potpourri of other endpoints 🍜
  //
  router->GetIndexEndpoint.Endpoint.use;
  router->FrontEndRoutes.use;
};