# ZalaxyMusic — Global Compliance Matrix

**Product:** ZalaxyMusic VST3 Synthesizer Plugin
**Entity:** Texas Sole Proprietorship (transitioning to LLC)
**Distribution:** Direct download from own website
**Model:** Free → Paid (one-time + optional subscription for cloud AI)
**AI:** Hybrid (local ONNX + cloud BYOK fallback)
**Last Updated:** 2026-11-01

---

## Jurisdiction Coverage

| Jurisdiction | Population | GDP | Priority | Status |
|--------------|------------|-----|----------|--------|
| United States (Texas) | 335M | $27T | Primary | ✅ Addressed |
| European Union (27) | 447M | $18T | High | ✅ Addressed |
| United Kingdom | 67M | $3.3T | High | ✅ Addressed |
| Australia | 26M | $1.7T | Medium | ✅ Addressed |
| China | 1.4B | $18T | Medium | ⚠️ Partial |
| Japan | 125M | $4.2T | Medium | ⚠️ Partial |
| South Korea | 52M | $1.8T | Medium | ⚠️ Partial |
| African Union (55) | 1.4B | $3T | Low | ⚠️ Partial |
| Mexico | 128M | $1.8T | Medium | ✅ Addressed |
| Saudi Arabia | 36M | $1.1T | Low | ⚠️ Partial |

---

## Compliance Requirements by Category

### 1. Consumer Protection & Refunds

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **US (TX)** | Texas Deceptive Trade Practices Act | No unfair practices | 14-day technical defect refund | None |
| **EU** | Consumer Rights Directive 2011/83/EU | 14-day withdrawal for digital content | Explicit consent to immediate download = withdrawal right lost; 14-day defect refund exceeds minimum | None |
| **UK** | Consumer Contracts Regulations 2013 | Same as EU | Same as EU | None |
| **Australia** | Australian Consumer Law | Consumer guarantees (fit for purpose) | 14-day defect refund + statutory guarantees apply | None |
| **Mexico** | Federal Consumer Protection Law | Right to refund for defects | 14-day defect refund | None |
| **China** | Consumer Rights Protection Law | 7-day no-reason return for online goods | Digital goods exception unclear; offer 14-day defect | ⚠️ Monitor |
| **Japan** | Act on Specified Commercial Transactions | 8-day cooling-off (door-to-door only) | Not applicable to digital downloads | None |
| **South Korea** | Consumer Protection in E-Commerce | 7-day withdrawal | Digital content exception if download started | ⚠️ Confirm |
| **Saudi Arabia** | E-Commerce Law | 7-day return for defects | 14-day defect refund covers | None |

### 2. Privacy & Data Protection

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **US (TX)** | No comprehensive law (sectoral) | Reasonable security, breach notice | TLS, encrypted key storage, no PII collected locally | None |
| **EU** | GDPR (2016/679) | Lawful basis, consent, DSR rights, DPA, DPIA | Consent for cloud AI; legitimate interest for updates/crashes; DSR process; no DPIA needed (low risk) | ⚠️ Need DPA with proxy server host |
| **UK** | UK GDPR + Data Protection Act 2018 | Same as EU + UK representative if no EU establishment | Same as EU | ⚠️ UK representative if >270 UK users |
| **Australia** | Privacy Act 1988 (APPs) | APP compliance, breach notification | TLS, minimal collection, DSR process | None |
| **China** | PIPL (2021) | Consent, data localization, security assessment | Consent for cloud; no China data localization (no China server) | ⚠️ If China users >1M, need SCC |
| **Japan** | APPI (2022 amend.) | Consent, purpose limitation, DSR | Consent for cloud; DSR process | None |
| **South Korea** | PIPA | Consent, data minimization, breach notice | Consent for cloud; minimal collection | None |
| **Mexico** | LFPDPPP | Consent, ARCO rights, privacy notice | Consent for cloud; DSR process | None |
| **Saudi Arabia** | PDPL (2023) | Consent, localization, DSR | Consent for cloud; no KSA localization | ⚠️ Monitor thresholds |

### 3. E-Commerce & Digital Services

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **EU** | Digital Services Act (DSA) | Transparency reporting, illegal content handling | DMCA agent, no user-generated content hosting (preset sharing only) | None |
| **EU** | ePrivacy Directive | Cookie consent | No cookies (desktop app) | None |
| **UK** | Online Safety Act | Illegal content, user safety | DMCA agent, preset moderation | ⚠️ If "user-to-user" service threshold met |
| **Australia** | Online Safety Act 2021 | Similar to UK | Same | ⚠️ Monitor |
| **China** | Cybersecurity Law | Data localization, security review | No China infrastructure | ⚠️ If targeting China market |
| **Saudi Arabia** | Cloud Computing Regulatory Framework | Local licensing for cloud | Not a cloud service provider | None |

### 4. Intellectual Property & DMCA

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **US** | DMCA §512 | Agent designation, repeat infringer policy | Agent designated, 3-strike policy | None (register with Copyright Office) |
| **EU** | Copyright Directive (2019/790) | Article 17 (upload filters) — only for large platforms | Not applicable (not a content-sharing platform) | None |
| **UK** | Copyright Directive retained | Same as EU | Same | None |
| **Australia** | Copyright Act 1968 | Safe harbor for ISPs | DMCA-style agent, takedown process | None |
| **Japan** | Copyright Act | Notice-and-takedown | Agent + process | None |
| **South Korea** | Copyright Act | Notice-and-takedown + 3-strike | Agent + process | None |

### 5. Accessibility

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **US** | ADA Title III (case law) | WCAG 2.1 AA for public accommodations | JUCE accessibility support; test with NVDA | ⚠️ Formal audit needed |
| **EU** | European Accessibility Act (2025) | WCAG 2.1 AA for digital products | Same | ⚠️ Audit before 2025 deadline |
| **UK** | Equality Act 2010 | Reasonable adjustments | Same | ⚠️ Audit |
| **Australia** | Disability Discrimination Act 1992 | WCAG 2.1 AA | Same | ⚠️ Audit |

### 6. Export Controls & Encryption

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **US** | EAR (15 CFR 730-774) | Self-classify encryption (TLS) | ECCN 5D002.c → NLR (no license) | File SNAP-R if >$1M revenue |
| **EU** | Dual-Use Regulation (2021/821) | Encryption controls | Same classification | None |
| **China** | Encryption Law | Import/license for encryption products | TLS only (standard) | None |

### 7. Payment & Tax

| Jurisdiction | Law | Requirement | Our Policy | Gap |
|--------------|-----|-------------|------------|-----|
| **US (TX)** | Sales tax nexus | Collect TX sales tax if TX buyer | Configure in Stripe/Paddle | ⚠️ Set up when paid |
| **EU** | VAT OSS (One-Stop Shop) | Collect VAT at buyer's rate | Use Paddle/Stripe Tax (merchant of record) | ⚠️ Set up when paid |
| **UK** | UK VAT | Collect UK VAT | Same | ⚠️ Set up when paid |
| **Australia** | GST | Collect 10% GST if >AUD 75k | Same | ⚠️ Set up when paid |
| **Japan** | Consumption Tax | 10% if >JPY 10M | Same | ⚠️ Set up when paid |

### 8. AI-Specific Regulations (Emerging)

| Jurisdiction | Law/Proposal | Requirement | Our Policy | Gap |
|--------------|--------------|-------------|------------|-----|
| **EU** | AI Act (2024/1689) | Transparency for AI systems (Art. 50) | Disclose AI-generated patches; local mode = no external AI | ⚠️ Document transparency notices |
| **US** | EO 14110 / State laws | AI transparency, bias testing | Local mode = transparent; cloud = provider responsibility | None |
| **China** | GenAI Measures (2023) | Security assessment, filing | Not providing GenAI service in China | None |
| **South Korea** | AI Act (pending) | High-risk AI obligations | Monitor | ⚠️ Track |

---

## Action Items by Priority

### Before First Paid Sale (Critical)
- [ ] Form Texas LLC ($300)
- [ ] Register DMCA agent with U.S. Copyright Office ($6)
- [ ] Set up merchant of record (Paddle recommended for global VAT)
- [ ] Configure tax collection (TX, EU OSS, UK, AU)
- [ ] Code-sign VST3 with EV certificate ($300/yr)
- [ ] Add About box with EULA/Privacy links in plugin

### Before Launch (High)
- [ ] Accessibility audit (WCAG 2.1 AA) — test with NVDA/VoiceOver
- [ ] Create website with EULA, Privacy, Refund, DMCA pages
- [ ] Proxy server for cloud AI (rate limiting, key hiding, logging)
- [ ] DPA with proxy host (if EU users)
- [ ] UK representative (if >270 UK users)
- [ ] Asset license tracker populated for all fonts/images/sounds

### Ongoing (Medium)
- [ ] Monitor China PIPL thresholds
- [ ] Monitor AI Act guidance (EU)
- [ ] Annual compliance review
- [ ] Update legal docs with version numbers

---

## Risk Flags (⚠️ = Needs Attention)

| Risk | Jurisdiction | Likelihood | Impact | Mitigation |
|------|--------------|------------|--------|------------|
| China PIPL localization | China | Low (no China infra) | High | Don't target China market initially |
| UK Online Safety Act scope | UK | Medium | Medium | Keep preset sharing minimal; no chat |
| EU AI Act transparency | EU | High | Low | Document "AI-generated" on patches |
| DPA with proxy host | EU/UK | High | Medium | Sign before processing EU data |
| Accessibility audit | US/EU/UK/AU | High | Medium | Budget $2-5k for formal audit |
| EV code-signing cost | Global | Certain | High | Budget $300/yr; standard cert + time alternative |

---

## Document Version Control

| Document | Version | Last Review | Next Review |
|----------|---------|-------------|-------------|
| EULA | 1.0 | 2026-11-01 | 2026-12-01 |
| Privacy Policy | 1.0 | 2026-11-01 | 2026-12-01 |
| Refund Policy | 1.0 | 2026-11-01 | 2026-12-01 |
| DMCA Agent | 1.0 | 2026-11-01 | 2026-12-01 |
| Compliance Matrix | 1.0 | 2026-11-01 | 2027-02-01 |

---

**Disclaimer:** This matrix is a compliance planning tool, not legal advice. Consult a qualified attorney in each jurisdiction before commercial launch.