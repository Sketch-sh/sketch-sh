/// <reference types="Cypress" />

import {
  assertErrorsOrWarnings,
  shortcut,
  typeTitle,
  typeBlock,
} from "../../helpers/editor_helpers";

const getId = url => {
  expect(url).to.match(/s\/.+/);
  return url.split("/")[4];
};

context("linking sketches", () => {
  let linkedSketch = "";
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

    cy.url().then(baseUrl => {
      linkedSketch = getId(baseUrl);
    });
  });
  it("link sketch", () => {
    cy.visit("new/reason");

    cy.get(".EditorNote__linkMenu").click();
    cy.get("#linkedLists").contains("Linked Links");
    cy.get(".EditorNote__linkMenu").eq(1).click();
    cy.get(".link__input")
      .eq(0)
      .type(linkedSketch);
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
    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");
    cy.url().then(baseUrl => {
      baseSketch = getId(baseUrl);
    });
  });

  it("refresh linked sketch", () => {
    cy.visit(`s/${linkedSketch}`);
    typeBlock(0, `let b = 1;`);
    shortcut("{ctrl}s");
    cy.visit(`s/${baseSketch}`);
    cy.get(".block__container")
      .first()
      .find(`button[aria-label="Add code block"]`)
      .click();
    typeBlock(1, `include Awesome;\n Awesome.a`);
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
  })
});