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
  it("click on new sketch in toolbar would create new reason sketch", () => {
    cy.visit("/");
    cy.get(`a[href="/new/reason"]`);
  });

  it("should have language switcher", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`).should("be.visible");
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

context("Language choosing page", () => {
  it("go to new -> click reason template", () => {
    cy.visit("new");
    cy.get(`a[id="re"]`).click();
    cy.location().should(loc => {
      expect(loc.pathname).to.eq("/new/reason");
    });
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");
  });

  it("go to new -> click ocaml template", () => {
    cy.visit("new");
    cy.get(`a[id="ml"]`).click();
    cy.location().should(loc => {
      expect(loc.pathname).to.eq("/new/ocaml");
    });
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=ML]")
      .should("be.checked");
  });

  it("/ml as an alias for ocaml", () => {
    cy.visit("ml");
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=ML]")
      .should("be.checked");
  });
  it("/re as an alias for reason", () => {
    cy.visit("re");
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");
  });
});
