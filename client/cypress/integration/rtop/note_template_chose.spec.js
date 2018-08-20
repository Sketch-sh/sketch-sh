/// <reference types="Cypress" />

context("Note template choose", () => {
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
});
