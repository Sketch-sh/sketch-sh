/// <reference types="Cypress" />

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertStdout,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
  assertLastBlockValue,
  typeTitle,
  typeBlock,
} from "../../helpers/editor_helpers";

context("linking sketches", () => {
  let baseSketch = "";
  it("create base sketch", () => {
    cy.visit("new/reason");
    typeTitle("Base sketch for linking");
    typeBlock(0, `let a = 1`);
    shortcut("{ctrl}s");
    cy.get(".block__container")
      .first()
      .find(".widget__value")
      .eq(0)
      .should("have.text", "let a: int = 1;\n");

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");

    let getId = url => {
      expect(url).to.match(/s\/.+/);
      return url.split("/")[4];
    };

    cy.url().then(baseUrl => {
      baseSketch = getId(baseUrl);
    });
  });
  it("link sketch", () => {
    cy.visit("new/reason");

    cy.get(".EditorNote__linkMenu").click();
    cy.get(".links__container").should("be.visible");
    cy.get(".link__input")
      .eq(0)
      .type(baseSketch);
    cy.get(".link__input")
      .eq(1)
      .type("Awesome");
    cy.get(".link__button")
      .first()
      .click();
    typeBlock(0, "include Awesome;");
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get(".block__container")
      .first()
      .find(".widget__value")
      .eq(0)
      .should("have.text", "let a: int = 1;\n");
  });
});
