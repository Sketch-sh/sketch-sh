/// <reference types="Cypress" />

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
} from "../../helpers/editor_helpers";

context("language toggle", () => {
  it("visual test - languge choose page", () => {
    cy.visit("new");

    cy.get(".Layout__center").matchImageSnapshot();
  });

  it("should have language switcher", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`)
      .should("be.visible")
      .matchImageSnapshot();
  });

  it("should be default to ReasonML", () => {
    cy.visit("new/reason");
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");
  });

  it("should persist language to database - ReasonML", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");

    cy.get(".block__container")
      .first()
      .as("block1")
      .find("textarea")
      .as("code1")
      .type(
        `let value = "awesome";
      switch (value) {
      | "awesome" => true
      | _ => false
      };`,
        { force: true }
      );
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get("@block1")
      .find(".widget__value")
      .eq(1)
      .should("have.text", "- : bool = true\n");

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");
    cy.reload();
    cy.get("@block1")
      .find(".widget__value")
      .eq(1)
      .should("have.text", "- : bool = true\n");
  });

  it("should persist language to database - OCaml", () => {
    cy.visit("new/reason");

    cy.get("input[id=ML]").check({ force: true });

    cy.get(".block__container")
      .first()
      .as("block1")
      .find("textarea")
      .as("code1")
      .type(
        `let value = "awesome" in
      match value with
      | "awesome" -> true
      | _ -> false`,
        { force: true }
      );
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get("@block1")
      .find(".widget__value")
      .eq(0)
      .should("have.text", "- : bool = true\n");

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");
    cy.reload();
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=ML]")
      .should("be.checked");
    cy.get("@block1")
      .find(".widget__value")
      .eq(0)
      .should("have.text", "- : bool = true\n");
  });

  it("should execute the code when language changed", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");

    cy.get(".block__container")
      .first()
      .as("block1")
      .find("textarea")
      .as("code1")
      .type(
        `let value = "awesome";
      switch (value) {
      | "awesome" => true
      | _ => false
      };`,
        { force: true }
      );
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get("input[id=ML]").check({ force: true });
    assertErrorsOrWarnings(1);
  });
});
